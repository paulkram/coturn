#include "prom_cpp_server.h"
#include "mainrelay.h"
#include "ns_turn_utils.h"

#if !defined(TURN_NO_PROMETHEUS) && defined(TURN_PROMETHEUS_CPP)

extern prometheus::Registry *registry;
extern prometheus::Exposer *exposer;

prometheus::Counter *stun_binding_request;
prometheus::Counter *stun_binding_response;
prometheus::Counter *stun_binding_error;

prometheus::Counter *turn_traffic_rcvp;
prometheus::Counter *turn_traffic_rcvb;
prometheus::Counter *turn_traffic_sentp;
prometheus::Counter *turn_traffic_sentb;

prometheus::Counter *turn_traffic_peer_rcvp;
prometheus::Counter *turn_traffic_peer_rcvb;
prometheus::Counter *turn_traffic_peer_sentp;
prometheus::Counter *turn_traffic_peer_sentb;

prometheus::Counter *turn_total_traffic_rcvp;
prometheus::Counter *turn_total_traffic_rcvb;
prometheus::Counter *turn_total_traffic_sentp;
prometheus::Counter *turn_total_traffic_sentb;

prometheus::Counter *turn_total_traffic_peer_rcvp;
prometheus::Counter *turn_total_traffic_peer_rcvb;
prometheus::Counter *turn_total_traffic_peer_sentp;
prometheus::Counter *turn_total_traffic_peer_sentb;

prometheus::Gauge *turn_total_allocations;

void start_prometheus_server(void) {
  if (turn_params.prometheus == 0) {
    TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "prometheus collector disabled, not started\n");
    return;
  }
  registry = new prometheus::Registry();

  exposer = new prometheus::Exposer("127.0.0.1:8080");

  // Create STUN counters
  stun_binding_request = BuildCounter()
                             .Name("stun_binding_request")
                             .Help("Incoming STUN Binding requests")
                             .Register(*registry);
  stun_binding_response = BuildCounter()
                             .Name("stun_binding_response")
                             .Help("Outgoing STUN Binding responses")
                             .Register(*registry);
  stun_binding_error = BuildCounter()
                             .Name("stun_binding_error")
                             .Help("STUN Binding errors")
                             .Register(*registry);

  // Create TURN traffic counter metrics
  turn_traffic_rcvp = BuildCounter()
                             .Name("turn_traffic_rcvp")
                             .Help("Represents finished sessions received packets")
                             .Register(*registry);
  turn_traffic_rcvb = BuildCounter()
                             .Name("turn_traffic_rcvb")
                             .Help("Represents finished sessions received bytes")
                             .Register(*registry);
  turn_traffic_sentp = BuildCounter()
                             .Name("turn_traffic_sentp")
                             .Help("Represents finished sessions sent packets")
                             .Register(*registry);
  turn_traffic_sentb = BuildCounter()
                             .Name("turn_traffic_sentb")
                             .Help("Represents finished sessions sent bytes")
                             .Register(*registry);

  // Create finished sessions traffic for peers counter metrics
  turn_traffic_peer_rcvp = BuildCounter()
                             .Name("turn_traffic_peer_rcvp")
                             .Help("Represents finished sessions peer received packets")
                             .Register(*registry);
  turn_traffic_peer_rcvb = BuildCounter()
                             .Name("turn_traffic_peer_rcvb")
                             .Help("Represents finished sessions peer received bytes")
                             .Register(*registry);
  turn_traffic_peer_sentp = BuildCounter()
                             .Name("turn_traffic_peer_sentp")
                             .Help("Represents finished sessions peer sent packets")
                             .Register(*registry);
  turn_traffic_peer_sentb = BuildCounter()
                             .Name("turn_traffic_peer_sentb")
                             .Help("Represents finished sessions peer sent bytes")
                             .Register(*registry);

  // Create total finished traffic counter metrics
  turn_total_traffic_rcvp = BuildCounter()
                             .Name("turn_total_traffic_rcvp")
                             .Help("Represents total finished sessions received packets")
                             .Register(*registry);
  turn_total_traffic_rcvb = BuildCounter()
                             .Name("turn_total_traffic_rcvb")
                             .Help("Represents total finished sessions received bytes")
                             .Register(*registry);
  turn_total_traffic_sentp = BuildCounter()
                             .Name("turn_total_traffic_sentp")
                             .Help("Represents total finished sessions sent packets")
                             .Register(*registry);
  turn_total_traffic_sentb = BuildCounter()
                             .Name("turn_total_traffic_sentb")
                             .Help("Represents total finished sessions sent bytes")
                             .Register(*registry);

  // Create total finished sessions traffic for peers counter metrics
  turn_total_traffic_peer_rcvp = BuildCounter()
                             .Name("turn_total_traffic_peer_rcvp")
                             .Help("Represents total finished sessions peer received packets")
                             .Register(*registry);
  turn_total_traffic_peer_rcvb = BuildCounter()
                             .Name("turn_total_traffic_peer_rcvb")
                             .Help("Represents total finished sessions peer received bytes")
                             .Register(*registry);
  turn_total_traffic_peer_sentp = BuildCounter()
                             .Name("turn_total_traffic_peer_sentp")
                             .Help("Represents total finished sessions peer sent packets")
                             .Register(*registry);
  turn_total_traffic_peer_sentb = BuildCounter()
                             .Name("turn_total_traffic_peer_sentb")
                             .Help("Represents total finished sessions peer sent bytes")
                             .Register(*registry);

  // Create total allocations number gauge metric
  turn_total_allocations = BuildGauge()
                             .Name("turn_total_allocations")
                             .Help("Represents current allocations number")
                             .Register(*registry);

  exposer->RegisterCollectable(registry);

  TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "prometheus collector started successfully\n");

  return;
}

void prom_set_finished_traffic(const char *realm, const char *user, unsigned long rsvp, unsigned long rsvb,
                               unsigned long sentp, unsigned long sentb, bool peer) {
  if (turn_params.prometheus == 1) {

    const prometheus::Labels label;
    label.insert({"realm", realm});

    if (turn_params.prometheus_username_labels) {
      label.insert({"user", user});
    }

    if (peer) {
      turn_traffic_peer_rcvp.Add(label).Increment(rsvp);
      turn_traffic_peer_rcvb.Add(label).Increment(rsvb);
      turn_traffic_peer_sentp.Add(label).Increment(sentp);
      turn_traffic_peer_sentb.Add(label).Increment(sentb);

      turn_total_traffic_peer_rcvp.Increment(rsvp);
      turn_total_traffic_peer_rcvb.Increment(rsvb);
      turn_total_traffic_peer_sentp.Increment(sentp);
      turn_total_traffic_peer_sentb.Increment(sentb);
    } else {
      turn_traffic_rcvp.Add(label).Increment(rsvp);
      turn_traffic_rcvb.Add(label).Increment(rsvb);
      turn_traffic_sentp.Add(label).Increment(sentp);
      turn_traffic_sentb.Add(label).Increment(sentb);

      turn_total_traffic_rcvp.Increment(rsvp);
      turn_total_traffic_rcvb.Increment(rsvb);
      turn_total_traffic_sentp.Increment(sentp);
      turn_total_traffic_sentb.Increment(sentb);
    }
  }
}

void prom_inc_allocation(SOCKET_TYPE type) {
  if (turn_params.prometheus == 1) {
    const prometheus::Labels label;
    label.insert({"type", {socket_type_name(type)}});
    turn_total_allocations.Add(label).Increment();
  }
}

void prom_dec_allocation(SOCKET_TYPE type) {
  if (turn_params.prometheus == 1) {
    const prometheus::Labels label;
    label.insert({"type", {socket_type_name(type)}});
    turn_total_allocations.Add(label).Decrement();
  }
}

void prom_inc_stun_binding_request(void) {
  if (turn_params.prometheus == 1) {
    stun_binding_request.Increment();
  }
}

void prom_inc_stun_binding_response(void) {
  if (turn_params.prometheus == 1) {
    stun_binding_response.Increment();
  }
}

void prom_inc_stun_binding_error(void) {
  if (turn_params.prometheus == 1) {
    stun_binding_error.Increment();
  }
}

void stop_prometheus_server(void) {
    delete exposer;
    delete registry;
}

#else

void start_prometheus_server(void) {
  TURN_LOG_FUNC(TURN_LOG_LEVEL_INFO, "turnserver compiled without prometheus support\n");
  return;
}

void stop_prometheus_server(void) {
    return;
}

#endif /* TURN_NO_PROMETHEUS && TURN_PROMETHEUS_CPP */
