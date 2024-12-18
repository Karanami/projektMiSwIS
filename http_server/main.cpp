#include <cstdlib>
#include <string>
#include <csignal>

#include "inc/mongoose.h"

// config

static const int s_debug_level =                MG_LL_INFO;

static const std::string s_root_dir =           ".";
static const std::string s_addr =               "0.0.0.0:8000";

// other

#define exit_log_err(args...)                   \
do                                              \
{                                               \
    MG_ERROR((args));                           \
    exit(EXIT_FAILURE);                         \
} while(0)

#define log_info(args...)  MG_INFO((args))

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

    static int i = 0;

    mg_http_reply(c, 200, "", "Api test response");

    i++;
}

static void api_get_temp(mg_connection *c, mg_http_message *hm)
{
    if(c == nullptr or hm == nullptr) exit_log_err("conneciton or http_message is null");

    
}

static void api_get_contamination(mg_connection *c, mg_http_message *hm)
{
    if(c == nullptr or hm == nullptr) exit_log_err("conneciton or http_message is null");
    

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
        else if(mg_match(hm->uri, mg_str("/api/get/temp"), NULL))
        {
            api_get_temp(c, hm);
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
    mg_mgr mgr;
    mg_mgr_init(&mgr);
   
    signal(SIGINT, signal_callback);
    signal(SIGTERM, signal_callback);

    if(mg_http_listen(&mgr, s_addr.c_str(), ev_callback, nullptr) == nullptr) exit_log_err("main: http listen failed on addr : %s", s_addr.c_str());
    
    log_info("Mongoose version : v%s", MG_VERSION);
    log_info("HTTP listener    : %s", s_addr.c_str());
    log_info("Web root         : [%s]", s_root_dir.c_str());

    while(s_signo == 0) mg_mgr_poll(&mgr, 1000); 
    
    mg_mgr_free(&mgr);

    log_info("Exiting on signal : %i", s_signo);
    
    return 0;
}
