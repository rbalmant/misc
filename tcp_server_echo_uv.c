#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <uv.h>

static uv_loop_t *loop;
static uv_tcp_t handle;

struct sockaddr addr;

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    buf->base = malloc(sizeof(uint8_t) * suggested_size);
    buf->len = suggested_size;
}

void on_close(uv_handle_t* handle)
{
    free(handle->data);
    free(handle);

    fprintf(stdout, "client disconnected.\n");
}

void on_write(uv_write_t* req, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "on_write error: %s", uv_strerror(status));
        return;
    }
    else
    {
        fprintf(stdout, "echo\n");
    }

    free(req->data);
    free(req);
}

void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
    if (nread >= 0)
    {
        uv_write_t *req = malloc(sizeof(uv_write_t));
        uv_write(req, (uv_stream_t *) client, buf, 1, on_write);
    }
    else
    {
        if (nread == UV_EOF)
        {
            uv_close((uv_handle_t *)client, on_close);
        }
        else
        {
            fprintf(stderr, "on_read error: %s\n", uv_strerror(nread));
        }
    }
}

void on_new_connection(uv_stream_t* server, int status)
{
    assert(server == (uv_stream_t*)&handle);

    if (status < 0)
    {
        fprintf(stderr, "on_new_connection error: %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);

    if (uv_accept((uv_stream_t *)&handle, (uv_stream_t *)client) == 0)
    {
        fprintf(stdout, "new client connected.\n");
        uv_read_start((uv_stream_t *) client, alloc_buffer, on_read);
    }
    else
    {
        uv_close((uv_handle_t *) client, on_close);
    }
}


int main(int argc, char* argv[]) {

    loop = uv_default_loop();

    int r=0;

    uv_tcp_init(loop, &handle);

    uv_ip4_addr("0.0.0.0", 30000, (struct sockaddr_in *) &addr);

    r = uv_tcp_bind(&handle, &addr, 0);
    if (r)
    {
        fprintf(stderr, "bind error: %s\n", uv_strerror(r));
        return -1;
    }

    r = uv_listen((uv_stream_t *)&handle, 511, on_new_connection);
    if (r)
    {
        fprintf(stderr, "listen error: %s\n", uv_strerror(r));
        return -2;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}
