//
// Created by gaspar on 25/10/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_SM_FRAMEWORK_H
#define FSM_PUMP_CONTROLLER_V1_SM_FRAMEWORK_H

typedef struct sm_context sm_context_t;
typedef void (*sm_handler_t) (sm_context_t *, void *data);


#define sm_declare_states(name, ...) \
typedef enum {                       \
    __VA_ARGS__,                     \
    name##_STATE_COUNT               \
    } name##_state_e

#define sm_declare_events(name, ...) \
typedef enum {                       \
    __VA_ARGS__,                     \
    name##_EVENT_COUNT               \
    } name##_event_e ;                \
static name##_event_e queuedEvent;                  \
static void* queuedData;

#define sm_declare_state_machine(name, st_init, ...) \
struct sm_context {                                  \
  void            *priv;                                   \
  name##_state_e  state;                           \
  name##_event_e  event;                           \
  sm_handler_t    *handler;                        \
};                                               \
static const unsigned int                        \
  name##_handler[name##_EVENT_COUNT][name##_STATE_COUNT] = \
  {__VA_ARGS__};                                   \
static sm_context_t name##_context =             \
  {0, st_init, (name##_event_e) 0,                 \
  (sm_handler_t *)name##_handler};                   \


#define sm_set_state(c, s) (c)->state = (s)
#define sm_set_private_data(c, p) (c)->priv = (p)
#define sm_get_private_data(c) (c)->priv

#define sm_define_handle_event(name) \
void name##_handle_event(name##_event_e ev, void* data) \
{ sm_context_t *c = &name##_context; \
  c->event = (ev);                   \
  c->handler[c->event * name##_STATE_COUNT + c->state](c, data); \
  return;                            \
  }

#define sm_declare_handle_event(name) \
void name##_handle_event(name##_event_e ev, void *data);

#define sm_handle_event(name, ev, data) name##_handle_event(ev, data);
#define sm_enqueue_event(name, ev, data) { queuedEvent = ev; queuedData = data;}
#define sm_handle_equeued_events(name) { \
    printf("HANDLING: %d", queuedEvent) ;                                    \
    if(queuedEvent < 255) sm_handle_event(name, queuedEvent, queuedData) ; \
    queuedEvent = 255; }

#endif //FSM_PUMP_CONTROLLER_V1_SM_FRAMEWORK_H
