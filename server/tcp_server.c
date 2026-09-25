#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include <string.h>
#include <tcp_echoserver.h>

#define TCP_SERVER_PORT  7          // Change this port if you want (e.g. 5000)

/* Private function prototypes */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void  tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);

/**
  * @brief  Initialize the TCP echo server
  */
void tcp_echoserver_init(void)
{
  struct tcp_pcb *pcb;

  /* Create a new TCP control block */
  pcb = tcp_new();
  if (pcb != NULL)
  {
    err_t err;

    /* Bind to any IP address and the defined port */
    err = tcp_bind(pcb, IP_ADDR_ANY, TCP_SERVER_PORT);
    if (err == ERR_OK)
    {
      /* Start listening for incoming connections */
      pcb = tcp_listen(pcb);

      /* Register the accept callback */
      tcp_accept(pcb, tcp_echoserver_accept);
    }
    else
    {
      /* Binding failed → free the PCB */
      memp_free(MEMP_TCP_PCB, pcb);
    }
  }
}

/**
  * @brief  Called when a new client connects
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* Set priority */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* Register callbacks for this connection */
  tcp_recv(newpcb, tcp_echoserver_recv);
  tcp_err(newpcb,  tcp_echoserver_error);
  tcp_poll(newpcb, tcp_echoserver_poll, 0);
  tcp_sent(newpcb, tcp_echoserver_sent);

  return ERR_OK;
}

/**
  * @brief  Called when data is received from the client
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  LWIP_UNUSED_ARG(arg);

  /* Remote closed the connection */
  if (p == NULL)
  {
    tcp_close(tpcb);
    return ERR_OK;
  }

  if (err != ERR_OK)
  {
    /* Error → free the buffer */
    if (p != NULL)
    {
      pbuf_free(p);
    }
    return err;
  }

  /* Echo the received data back to the client */
  if (tcp_write(tpcb, p->payload, p->tot_len, TCP_WRITE_FLAG_COPY) == ERR_OK)
  {
    tcp_output(tpcb);   // Force immediate send
  }

  /* Tell LwIP that we have processed the data (update receive window) */
  tcp_recved(tpcb, p->tot_len);

  /* Free the received pbuf */
  pbuf_free(p);

  return ERR_OK;
}

/**
  * @brief  Error callback
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);
  /* PCB is already freed by LwIP */
}

/**
  * @brief  Poll callback (called periodically)
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(tpcb);
  return ERR_OK;
}

/**
  * @brief  Called when data has been successfully sent
  */
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(tpcb);
  LWIP_UNUSED_ARG(len);
  return ERR_OK;
}
/*This is in Core/src*/