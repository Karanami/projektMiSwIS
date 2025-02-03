//#include <stdlib>
//#include <iomanip>
//#include <mutex>
#include <string.h>
//#include <csignal>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <fcntl.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include "mongoose.h"

// config

// static const int s_debug_level =                MG_LL_INFO;

static const char* s_root_dir =           ".";
static const char* s_addr =               "0.0.0.0:8000";

// server info

static char* s_alive_since = "";
static char* s_server_version = "v1.0";

// com port info

static char * s_com_port = "/dev/ttyRPMSG0"; //;"/dev/pts/3"; //

struct termios tty;

int serial_port;

//pid
double want_temp = 24.0;
double want_dirt = 50.0;
double max_watt = 2000.0;
double min_watt = 0.0;

double max_int = 100.0;
double min_int = -100.0;

double kp = 200.f;
double ki = 1.f;
double kd = 0.1f;

// data
static float temp = 0;
static float dirt = 0;

// other

#define exit_log_err(args...)   do { MG_ERROR((args)); exit(EXIT_FAILURE); } while(0)

#define log_info(args...)  MG_INFO((args))

// interrupt handling

static int s_signo;
static void signal_callback(int signo)
{
    s_signo = signo;
}

// pid cd.

static double update_pid(double dt)
{
    dt *= 20.f;

    static double prev_error = 0.0;
    static double integral = 0.0;
    double error = want_temp - temp;
    integral += error * dt;

    if(integral > max_int) integral = max_int;
    if(integral < min_int) integral = min_int;

    double derivative = (error - prev_error) / dt;
    double control = kp * error + ki * integral + kd * derivative;
    if(control > max_watt) control = max_watt;
    if(control < min_watt) control = min_watt;

    prev_error = error;

    //log_info("dt: %f, ctrl: %f, err: %f, der: %f, int: %f, temp: %f", dt, control, error, derivative, integral, temp);

    return control;
}

// api

static void api_test(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    mg_http_reply(c, 200, "", "{ \"server_version\" : \"%s\", \"alive_since\" : \"%s\" }", s_server_version, s_alive_since);
}

static void api_get_temperature(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");
    
    mg_http_reply(c, 200, "", "{ \"temperature\" : %f }", temp);
}

static void api_get_contamination(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    mg_http_reply(c, 200, "", "{ \"contamination\" : %f }", dirt);
}

static void api_set_flow(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    double num;
    if(mg_json_get_num(hm->body, "$[0]", &num))
    {
        mg_http_reply(c, 201, "", "{ \"Status\" : \"Ok\" }");
        char msg[32];
        sprintf(msg, "SET:FLOW:%d", (int)(num * 10));
        log_info(msg);
        write(serial_port, msg, strlen(msg));
    }
    else 
    {
        mg_http_reply(c, 500, NULL, "Parameters missing\n");
    }
}

static void api_set_watt(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    double num;
    if(mg_json_get_num(hm->body, "$[0]", &num))
    {
        mg_http_reply(c, 201, "", "{ \"Status\" : \"Ok\" }");
        char msg[32];
        sprintf(msg, "SET:WATT:%d", (int)(num * 10));
        log_info(msg);
        write(serial_port, msg, strlen(msg));
    }
    else 
    {
        mg_http_reply(c, 500, NULL, "Parameters missing\n");
    }
}

static void api_set_want_temp(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    double num;
    if(mg_json_get_num(hm->body, "$[0]", &num))
    {
        mg_http_reply(c, 201, "", "{ \"Status\" : \"Ok\" }");
        want_temp = num;
        log_info("want_temp = %f", want_temp);
    }
    else 
    {
        mg_http_reply(c, 500, NULL, "Parameters missing\n");
    }
}

static void api_set_want_dirt(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    double num;
    if(mg_json_get_num(hm->body, "$[0]", &num))
    {
        mg_http_reply(c, 201, "", "{ \"Status\" : \"Ok\" }");
        want_dirt = num;
        log_info("want_dirt = %f", want_dirt);
    }
    else 
    {
        mg_http_reply(c, 500, NULL, "Parameters missing\n");
    }
}

static void api_get_want_temp(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    mg_http_reply(c, 200, "", "{ \"wantTemp\" : %f }", want_temp);
}

static void api_get_want_dirt(struct mg_connection *c, struct mg_http_message *hm)
{
    if(c == NULL || hm == NULL) exit_log_err("conneciton or http_message is null");

    mg_http_reply(c, 200, "", "{ \"wantDirt\" : %f }", want_dirt);
}

// mongoose

static void ev_callback(struct mg_connection *c, int ev, void* ev_data)
{
    if(ev == MG_EV_HTTP_MSG)
    {
        if(c == NULL || ev_data == NULL) exit_log_err("conneciton or ev_data is null");
        struct mg_http_message *hm = (struct mg_http_message*)ev_data;
            
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
        else if(mg_match(hm->uri, mg_str("/api/set/flow"), NULL)) 
        {
            api_set_flow(c, hm);
        }
        else if(mg_match(hm->uri, mg_str("/api/set/watt"), NULL)) 
        {
            api_set_watt(c, hm);
        }
        else if(mg_match(hm->uri, mg_str("/api/get/want_temp"), NULL)) 
        {
            api_get_want_temp(c, hm);
        }
        else if(mg_match(hm->uri, mg_str("/api/get/want_dirt"), NULL)) 
        {
            api_get_want_dirt(c, hm);
        }
        else if(mg_match(hm->uri, mg_str("/api/set/want_temp"), NULL)) 
        {
            api_set_want_temp(c, hm);
        }
        else if(mg_match(hm->uri, mg_str("/api/set/want_dirt"), NULL)) 
        {
            api_set_want_dirt(c, hm);
        }
        else // this is unsafe but i dont care
        {
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = s_root_dir;
            mg_http_serve_dir(c, hm, &opts);
        }
        
        //log_info("%.*s %.*s %lu -> %.*s %lu", hm->method.len, hm->method.buf, hm->uri.len, hm->uri.buf, hm->body.len, 3, c->send.buf + 9, c->send.len);
    }
}

// main

int main (int argc, char *argv[]) 
{
    serial_port = open(s_com_port, O_RDWR);

    if (serial_port < 0) 
    {
        log_info("Error opening serial port, error %d", errno);
    }
    else
    {
        tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~CRTSCTS;
        tty.c_cflag |= CREAD | CLOCAL;
        tty.c_lflag &= ~ICANON;
        //tty.c_lflag |= ICANON;
        tty.c_lflag &= ~ECHO; // Disable echo
        tty.c_lflag &= ~ECHOE; // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG;
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
        tty.c_cc[VTIME] = 5;    // Wait for up to 0.5s (10 deciseconds), returning as soon as any data is received.
        tty.c_cc[VMIN] = 1;

        cfsetispeed(&tty, B115200);
        cfsetospeed(&tty, B115200);

        int flags = fcntl(serial_port, F_GETFL, 0);
        if(flags == -1) exit_log_err("Error getting serial port flags");
        
        fcntl(serial_port, F_SETFL, flags | O_NONBLOCK);

        if(tcgetattr(serial_port, &tty) != 0)
        {
            exit_log_err("Error setting up serial port, error %d", errno);
        }
    }

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
   
    signal(SIGINT, signal_callback);
    signal(SIGTERM, signal_callback);

    if(mg_http_listen(&mgr, s_addr, ev_callback, NULL) == NULL)
    {
        exit_log_err("http listen failed on addr : %s", s_addr);
    }
    
    //s_alive_since = get_time();

    log_info("Mongoose version : v%s", MG_VERSION);
    log_info("HTTP listener    : %s", s_addr);
    log_info("Web root         : [%s]", s_root_dir);
    //log_info("Web start        : %s", s_alive_since);

    char read_buf[32];

    struct timeval prev_time, time;
    double dt;

    gettimeofday(&prev_time, NULL);

    while(s_signo == 0) 
    {
        gettimeofday(&time, NULL);
        dt =   (time.tv_sec  - prev_time.tv_sec )  * 1.0;
        dt +=  (time.tv_usec - prev_time.tv_usec ) / 1000000.0;
        prev_time = time;
        double watt = update_pid(dt);

        double flow = 0.0f;

        if(dirt > want_dirt) flow = 10.f;
        else flow = 0.f;

        mg_mgr_poll(&mgr, 100);

        if(serial_port >= 0)
        {
            char msg0[32];
            sprintf(msg0, "SET:WATT:%d", (int)(watt * 10.0));
            write(serial_port, msg0, strlen(msg0));
            usleep(50000);

            sprintf(msg0, "SET:FLOW:%d", (int)(flow * 10));
            write(serial_port, msg0, strlen(msg0));
            usleep(50000);

            int n;
            
            memset(read_buf, 0, 32);

            char msg1[] = "GET:TEMP";
            write(serial_port, msg1, strlen(msg1));
            usleep(50000);
            n = read(serial_port, read_buf, sizeof(read_buf));
            usleep(50000);
            
            log_info(read_buf);

            if(n < 0 || n >= 32);// log_info("com port read error"); //n == 32 => error?
            else if(n > 0)
            {
                read_buf[n] = '\0'; //na wszelki wypadek
                temp = (float)atof(read_buf);
            }

            memset(read_buf, 0, 32);

            char msg2[] = "GET:DIRT";
            write(serial_port, msg2, strlen(msg2));
            usleep(50000);
            n = read(serial_port, read_buf, sizeof(read_buf));
            usleep(50000);

            log_info(read_buf);

            if(n < 0 || n >= 32);// log_info("com port read error"); //n == 32 => error?
            else if(n > 0)
            {
                read_buf[n] = '\0'; //na wszelki wypadek
                dirt = (float)atof(read_buf);
            }
        }
    }
    
    mg_mgr_free(&mgr);

    log_info("Exiting on signal : %d", s_signo);

    close(serial_port);
    
    return 0;
}
