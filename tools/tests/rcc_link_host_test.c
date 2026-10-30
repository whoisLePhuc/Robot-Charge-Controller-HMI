#include <assert.h>
#include <stdio.h>
#include "../../firmware/src/rcc_link.c"
static void reset(void) {
 memset(&s_model,0,sizeof(s_model)); memset(&s_start,0,sizeof(s_start)); memset(&s_stop,0,sizeof(s_stop));
 s_model_lock=(void*)1; s_start_requested=false; s_stop_requested=false;
 s_model.online=true; test_now=1000000;
}
int main(void) {
 uint8_t status[48]={0}; reset();
 assert(!rcc_link_request_start());
 status[18]=0x0f; parse_reply_data(CMD_STATUS,status,48,test_now);
 assert(s_model.simulated_measurements && !s_model.simulated_ready && s_model.charging_established);
 assert(!rcc_link_request_start());
 status[18]=0x18; parse_reply_data(CMD_STATUS,status,48,test_now);
 assert(s_model.simulated_ready && !s_model.charging_established);
 assert(rcc_link_request_start()); s_start_requested=false;
 test_now+=1500000; assert(!rcc_link_request_start());
 parse_reply_data(CMD_STATUS,status,47,test_now); assert(!rcc_link_request_start());
 parse_reply_data(CMD_STATUS,status,48,test_now); assert(rcc_link_request_start()); s_start_requested=false;
 status[18]=0; parse_reply_data(CMD_STATUS,status,48,test_now);
 assert(!s_model.simulated_measurements && !s_model.simulated_ready); assert(rcc_link_request_start()); s_start_requested=false;
 status[18]=0x18;parse_reply_data(CMD_STATUS,status,48,test_now);
 s_model.device_valid=true; uint8_t device[40]={1,0,1}; device[20]=1;
 parse_reply_data(CMD_DEVICE,device,40,test_now);
 assert(!s_model.status_valid && !s_model.simulated_measurements && !s_model.simulated_ready);
 assert(!rcc_link_request_start()); assert(rcc_link_request_stop());
 reset(); status[18]=0x18;
 s_query.active=true; s_query.id=7; s_query.command=CMD_STATUS;
 rcc_message_t message={0}; message.message_type=2; message.request_id=7;
 message.command=CMD_STATUS; message.payload_length=60; message.payload[0]=2;
 memcpy(message.payload+12,status,48);
 handle_result(&message,test_now);
 assert(s_model.status_valid && s_model.simulated_measurements && s_model.simulated_ready);
 rcc_snapshot_t snapshot; rcc_link_snapshot(&snapshot);
 assert(snapshot.simulated_measurements && snapshot.simulated_ready && snapshot.status_age_ms==0);
 test_now+=1499000; assert(rcc_link_request_start()); s_start_requested=false;
 test_now+=1000; assert(!rcc_link_request_start());
 s_model.status_valid=false; assert(!rcc_link_request_start());
 s_model.status_valid=true; test_now-=1500000;
 s_start.active=true; assert(!rcc_link_request_start()); s_start.active=false;
 s_stop.active=true; assert(!rcc_link_request_start());
 puts("rcc_link host tests passed");
}
