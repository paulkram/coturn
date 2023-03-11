
#ifndef __PROM_CPP_SERVER_H__
#define __PROM_CPP_SERVER_H__

#define DEFAULT_PROM_SERVER_PORT (9641)

#if !defined(TURN_NO_PROMETHEUS) && !defined(TURN_NO_PROMETHEUS_CPP)

#include "ns_turn_ioalib.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/gauge.h>
#include <prometheus/registry.h>

#ifdef __cplusplus
} // extern "C"
#endif

extern prometheus::Registry *registry;
extern prometheus::Exposer *exposer;

extern prometheus::Counter *stun_binding_request;
extern prometheus::Counter *stun_binding_response;
extern prometheus::Counter *stun_binding_error;

extern prometheus::Counter *turn_new_allocation;
extern prometheus::Counter *turn_refreshed_allocation;
extern prometheus::Counter *turn_deleted_allocation;

extern prometheus::Counter *turn_traffic_rcvp;
extern prometheus::Counter *turn_traffic_rcvb;
extern prometheus::Counter *turn_traffic_sentp;
extern prometheus::Counter *turn_traffic_sentb;

extern prometheus::Counter *turn_traffic_peer_rcvp;
extern prometheus::Counter *turn_traffic_peer_rcvb;
extern prometheus::Counter *turn_traffic_peer_sentp;
extern prometheus::Counter *turn_traffic_peer_sentb;

extern prometheus::Counter *turn_total_traffic_rcvp;
extern prometheus::Counter *turn_total_traffic_rcvb;
extern prometheus::Counter *turn_total_traffic_sentp;
extern prometheus::Counter *turn_total_traffic_sentb;

extern prometheus::Counter *turn_total_traffic_peer_rcvp;
extern prometheus::Counter *turn_total_traffic_peer_rcvb;
extern prometheus::Counter *turn_total_traffic_peer_sentp;
extern prometheus::Counter *turn_total_traffic_peer_sentb;

extern prometheus::Gauge *turn_total_allocations_number;

#define TURN_ALLOC_STR_MAX_SIZE (20)

#ifdef __cplusplus
extern "C" {
#endif

void start_prometheus_server(void);

void prom_set_finished_traffic(const char *realm, const char *user, unsigned long rsvp, unsigned long rsvb,
                               unsigned long sentp, unsigned long sentb, bool peer);

void prom_inc_allocation(SOCKET_TYPE type);
void prom_dec_allocation(SOCKET_TYPE type);

void prom_inc_stun_binding_request(void);
void prom_inc_stun_binding_response(void);
void prom_inc_stun_binding_error(void);

#else

void start_prometheus_server(void);

#endif /* TURN_NO_PROMETHEUS && TURN_NO_PROMETHEUS_CPP */

void stop_prometheus_server(void);

#ifdef __cplusplus
}
#endif /* __clplusplus */

#endif /* __PROM_CPP_SERVER_H__ */
