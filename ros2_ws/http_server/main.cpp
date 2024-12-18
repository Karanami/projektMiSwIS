#include <cstdlib>
#include <iomanip>
#include <mutex>
#include <string>
#include <csignal>
#include <ctime>
#include "rclcpp/rclcpp.hpp"

#include "include/mongoose.h"

// config

static const int s_debug_level =                MG_LL_INFO;

static const std::string s_root_dir =           ".";
static const std::string s_addr =               "0.0.0.0:8000";

// server info

static std::string s_alive_since = "";
static std::string s_server_version = "v1.0";

// other

#define exit_log_err(args...)   do { MG_ERROR((args)); exit(EXIT_FAILURE); } while(0)

#define log_info(args...)  MG_INFO((args))

static std::string get_time()
{
    time_t t;
    char *ct;
    std::time(&t);
    ct = ctime(&t);

    if(ct != nullptr) 
    {
        std::string st(ctime(&t));
        st.pop_back();
        return st;
    }
    return "NaN";
}

// interrupt handling

static int s_signo;
static void signal_callback(int signo)
{
    s_signo = signo;
}

// api

static void api_test(mg_connection *c, mg_http_message *hm)
{
    if(c == nullptr or hm == nullptr) exit_log_err("conneciton or http_message is null");

    mg_http_reply(c, 200, "", R"({ "server_version" : "%s", "alive_since" : "%s" })", s_server_version.c_str(), s_alive_since.c_str());
}

static void api_get_temperature(mg_connection *c, mg_http_message *hm)
{
    if(c == nullptr or hm == nullptr) exit_log_err("conneciton or http_message is null");

    mg_http_reply(c, 200, "", R"({ "temperature" : 0.0 })");
}

static void api_get_contamination(mg_connection *c, mg_http_message *hm)
{
    if(c == nullptr or hm == nullptr) exit_log_err("conneciton or http_message is null");

    mg_http_reply(c, 200, "", R"({ "contamination" : 0 })");
}

// mongoose

static void ev_callback(mg_connection *c, int ev, void* ev_data)
{
    if(ev == MG_EV_HTTP_MSG)
    {
        if(c == nullptr or ev_data == nullptr) exit_log_err("conneciton or ev_data is null");
        mg_http_message *hm = (mg_http_message*)ev_data;
            
        if(mg_match(hm->uri, mg_str("/api/test"), NULL)) 
        {
            api_test(c, hm);
        }
        else if(mg_match(hm->uri, mg_str("/api/get/temperature"), NULL))
        {
            api_get_temperature(c, hm);
        }
        else if(mg_match(hm->uri, mg_str("/api/get/contamination"), NULL))
        {
            api_get_contamination(c, hm);
        }
        else // this is unsafe but i dont care
        {
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = s_root_dir.c_str();
            mg_http_serve_dir(c, hm, &opts);
        }
        
        log_info("%.*s %.*s %lu -> %.*s %lu", hm->method.len, hm->method.buf, hm->uri.len, hm->uri.buf, hm->body.len, 3, c->send.buf + 9, c->send.len);
    }
}

// main

int main (int argc, char *argv[]) 
{
    rclcpp::init(argc, argv);

    mg_mgr mgr;
    mg_mgr_init(&mgr);
   
    signal(SIGINT, signal_callback);
    signal(SIGTERM, signal_callback);

    if(mg_http_listen(&mgr, s_addr.c_str(), ev_callback, nullptr) == nullptr) exit_log_err("http listen failed on addr : %s", s_addr.c_str());
    
    s_alive_since = get_time();

    log_info("Mongoose version : v%s", MG_VERSION);
    log_info("HTTP listener    : %s", s_addr.c_str());
    log_info("Web root         : [%s]", s_root_dir.c_str());
    log_info("Web start        : %s", s_alive_since.c_str());

    while(s_signo == 0) mg_mgr_poll(&mgr, 1000); 
    
    log_info("Exiting on signal : %i", s_signo);

    rclcpp::shutdown();
    mg_mgr_free(&mgr);
    
    return 0;
}
