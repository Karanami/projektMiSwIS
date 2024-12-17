#include <cstdlib>
#include <string>
#include <csignal>

#include "inc/mongoose.h"

// config

static const int s_debug_level =                MG_LL_INFO;

static const std::string s_root_dir =           ".";
static const std::string s_addr =               "0.0.0.0:8000";

// other
template < class ... Ts >
static void exit_log_err(std::string err_msg, Ts ... args)
{
    MG_ERROR((err_msg.c_str(), args... ));
    exit(EXIT_FAILURE);
}

template < class ... Ts >
static void log_info(std::string msg, Ts ... args)
{
    MG_INFO((msg.c_str(), args...));
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
    if(c == nullptr or hm == nullptr) exit_log_err("api_test: conneciton or http_message is null"); 
}

// mongoose

static void ev_callback(mg_connection *c, int ev, void* ev_data)
{
    if(ev == MG_EV_HTTP_MSG)
    {
        if(ev_data == nullptr) exit_log_err("ev callback: ev_data is null");
        mg_http_message *hm = (mg_http_message*)ev_data;
            
        if(mg_match(hm->uri, mg_str("/api/test"), NULL)) 
        {
            api_test(c, hm);
        }
        else // this is unsafe but i dont care
        {
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = s_root_dir.c_str();
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

// main

int main (int argc, char *argv[]) 
{
    mg_mgr mgr;
    mg_mgr_init(&mgr);
    
    if(mg_http_listen(&mgr, s_addr.c_str(), ev_callback, nullptr) == nullptr) exit_log_err("main: http listen failed on addr : %s", s_addr.c_str());
    
    log_info("Mongoose version : v%s", MG_VERSION);
    log_info("HTTP listener    : %s", s_addr.c_str());
    log_info("Web root         : [%s]", s_root_dir.c_str());

    while(s_signo == 0) mg_mgr_poll(&mgr, 1000); 
    
    mg_mgr_free(&mgr);

    log_info("Exiting on signal : %i", s_signo);
    
    return 0;
}
