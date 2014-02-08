/**
 * @author helloworld
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <string.h>

#define hw_strcpy(d, s) strcpy((char *) d, (const char *) s);

#define hw_pstrfromchar(p, ns, c) \
  (ns).data = ngx_pcalloc(p, ((ns).len = ngx_strlen(c)) + 1); \
  hw_strcpy((ns).data, c)

#define OUTPUT_STRING   "hello, world!";

static char         *ngx_http_helloworld(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t     ngx_http_helloworld_handler(ngx_http_request_t *r);

static ngx_int_t ngx_http_helloworld_init_process(ngx_cycle_t *cycle);
static void      ngx_http_helloworld_exit_process(ngx_cycle_t *cycle);
static ngx_int_t helloworld_output_builder(ngx_http_request_t *req, ngx_chain_t *out, ngx_int_t status);

static ngx_command_t  ngx_http_helloworld_commands[] = {
    {
        ngx_string("helloworld"),
        NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
        ngx_http_helloworld,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t  ngx_http_helloworld_module_ctx = {
    NULL,   /* preconfiguration */
    NULL,   /* postconfiguration */
    NULL,   /* create main configuration */
    NULL,   /* init main configuration */
    NULL,   /* create server configuration */
    NULL,   /* merge server configuration */
    NULL,   /* create location configuration */
    NULL    /* create location configuration */
};

ngx_module_t  ngx_http_helloworld_module = {
    NGX_MODULE_V1                   ,
    &ngx_http_helloworld_module_ctx ,   /* module context */
    ngx_http_helloworld_commands    ,   /* module directives */
    NGX_HTTP_MODULE                 ,   /* module type */
    NULL                            ,   /* init master */
    NULL                            ,   /* init module */
    ngx_http_helloworld_init_process,   /* init process */
    NULL                            ,   /* init thread */
    NULL                            ,   /* exit thread */
    ngx_http_helloworld_exit_process,   /* exit process */
    NULL                            ,   /* exit master */
    NGX_MODULE_V1_PADDING
};

static char *ngx_http_helloworld(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t    *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_helloworld_handler;

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_helloworld_handler(ngx_http_request_t *req)
{
    ngx_chain_t     out;

    ngx_log_error(NGX_LOG_DEBUG, req->connection->log, 0, "***Call helloworld Handler***");

    (void)helloworld_output_builder(req, &out, NGX_HTTP_OK);

    return ngx_http_output_filter(req, &out);
}

ngx_int_t ngx_http_helloworld_init_process(ngx_cycle_t *cycle)
{
    ngx_log_error(NGX_LOG_DEBUG, cycle->log, 0, "** init process **");
    return NGX_OK;
}

void ngx_http_helloworld_exit_process(ngx_cycle_t *cycle)
{
    ngx_log_error(NGX_LOG_DEBUG, cycle->log, 0, "** exit process **");
}

static ngx_int_t helloworld_output_builder(
    ngx_http_request_t  *req,
    ngx_chain_t         *out,
    ngx_int_t            status)
{
    ngx_int_t            rc;
    ngx_buf_t           *buffmng;

    if (req->method == NGX_HTTP_HEAD)
    {
        rc = ngx_http_send_header(req);
        if (rc == NGX_ERROR || rc > NGX_OK || req->header_only) {
            return rc;
        }
    }

    buffmng = ngx_pcalloc(req->pool, sizeof(ngx_buf_t));
    if (buffmng == NULL)
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    out->buf  = buffmng;
    out->next = NULL;

    buffmng->pos      = (unsigned char *)OUTPUT_STRING;
    buffmng->last     = (buffmng->pos + (strlen(OUTPUT_STRING) - 1));
    buffmng->memory   = 1;
    buffmng->last_buf = 1;

    hw_pstrfromchar(req->pool, req->headers_out.content_type, "text/html; charset=UTF-8");
    req->headers_out.status            = status;
    req->headers_out.content_length_n  = strlen(OUTPUT_STRING);

    rc = ngx_http_send_header(req);
    if (rc == NGX_ERROR || rc > NGX_OK || req->header_only)
    {
        return rc;
    }

    return NGX_OK;
}
