#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/packetbuf.h" // For getting data from the packet
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/simple-udp.h"
#include "net/routing/rpl-lite/rpl-neighbor.h"
#include "net/link-stats.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include "sys/clock.h"

// for getting parent and neighbour info
#include "net/routing/rpl-lite/rpl.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
PROCESS(neighbor_process, "Neighbour process");
AUTOSTART_PROCESSES(&udp_client_process, &neighbor_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");
  rx_count++;
}

void
input_callback() {
  // const uint8_t *data = (uint8_t *) packetbuf_dataptr();
  uint16_t datalen = (uint16_t) packetbuf_datalen();
  const linkaddr_t *source = packetbuf_addr(PACKETBUF_ADDR_SENDER);
  // const linkaddr_t *destination = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);

  const struct link_stats *incoming_link_stats = link_stats_from_lladdr(source);

  printf("Inspected packet with RSSI %d, data = ", incoming_link_stats->rssi);  

  uint16_t i;
  /* Printing the data characters of the packet*/
  for(i = 0; i < datalen; i++) {
    printf("%c", ((char *)packetbuf_dataptr())[i]);
  }
  printf("\n");

  // printing link stats


  /* Printing individual bytes of the packet */
  for(i = 0; i < datalen; i++) {
    printf("%02x ", ((uint8_t *)packetbuf_dataptr())[i]);
  }
  printf("\n");
}

void
output_callback(int mac_status) {

}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;
  static uint32_t tx_count;
  static uint32_t missed_tx_count;

  /* sensor data */
  int water_level=0, moisture=0;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  /* Add the packet sniffer */
  NETSTACK_SNIFFER(sniffer, &input_callback, &output_callback);
  netstack_sniffer_add(&sniffer);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if(NETSTACK_ROUTING.node_is_reachable() &&
        NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

      /* Print statistics every 10th TX */
      if(tx_count % 10 == 0) {
        LOG_INFO("Tx/Rx/MissedTx: %" PRIu32 "/%" PRIu32 "/%" PRIu32 "\n",
                 tx_count, rx_count, missed_tx_count);
      }

      /* Send to DAG root */
      LOG_INFO("Sending request %"PRIu32" to ", tx_count);
      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO_("\n");

      /* Finding random sensor values */
      int water_level_min = 30, water_level_range = 10, moisture_min = 80, moisture_range = 15;
      water_level = ((float)rand()/(float)(RAND_MAX)) * water_level_range + water_level_min;
      moisture = ((float)rand()/(float)(RAND_MAX)) * moisture_range + moisture_min;

      /* Sending request to udp-server */
      LOG_INFO("Sending data %ld %d %d to ", clock_time(), water_level, moisture);
      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO_("\n");

      snprintf(str, sizeof(str), "%ld %d %d", clock_time(), water_level, moisture);
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      tx_count++;
    } else {
      LOG_INFO("Not reachable yet\n");
      if(tx_count > 0) {
        missed_tx_count++;
      }
    }

  // // Get parent IP addr and print it
  // const uip_ipaddr_t *parent_ip = rpl_get_parent_ipaddr();
  // printf("Parent IP address: ");
  // uip_debug_ipaddr_print(parent_ip);
  // printf("\n");

  /* Printing neighbours */
  // uip_ds6_route_t *route = uip_ds6_route_head();
  // while(route != NULL) {
  //   printf("%d", *route);
  //   route = uip_ds6_route_next(route);
  //   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer_2));
  // }

  // while(1) {
  //   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer_2));
  //   rpl_neighbor_print_list("\0");
  // }

  // rpl_neighbor_print_list("\0");

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(neighbor_process, ev, data)
{
  static struct etimer periodic_timer;
  int number_of_neighbors;

  PROCESS_BEGIN();

  // set etimer
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

  while (1) {
    // wait for etimer to expire
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    // get neighbor count and print it
    number_of_neighbors = rpl_neighbor_count();
    printf("Number of neighbours: %d\n", number_of_neighbors);

    // print list of neighbors (IN PROGRESS)
    rpl_neighbor_print_list("UDP client mote");

    // timer with jitter
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/