#include <stdio.h>
#include "common.h"
#include "../coder.h"
#include "../coder.cpp"

void coder_main() {
	test("crc8", []() {
		uint8_t data[] { 0xBA, 0xDF, 0x00, 0xD4, 0xAD, 0xEA, 0xDF, 0x00 };
		assert(crc8(data, 0) == 0);
		assert(crc8(data, sizeof(data)) == 13);
		assert(crc8(data, 2) == 85);
	});

  test("code_receiver", []() {
    CodeReceiver c(10, 1);
    
    c.did_read_at(false, 1000);
    c.did_read_at(false, 1001);
    c.did_read_at(false, 1008);
    c.did_read_at(true, 1009); // 0!
    c.did_read_at(true, 1024);
    c.did_read_at(false, 1033); // 1
    c.did_read_at(true, 1040); // 0
    c.did_read_at(false, 1050); // 0
    c.did_read_at(true, 1070); // 1
    c.did_read_at(false, 1090); // 1
    c.did_read_at(true, 1100); // 0 
    c.did_read_at(false, 1110); // 0
    c.did_read_at(true, 1130); // 1

    c.did_read_at(false, 1150); // 1
    c.did_read_at(true, 1160); // 0
    c.did_read_at(false, 1170); // 0
    c.did_read_at(true, 1180); // 0
    c.did_read_at(false, 1200); // 1
    c.did_read_at(true, 1220); // 1
    assert(!c.did_read_at(false, 1230)); // 0
    assert(c.did_read_at(true, 1250)); // 1

    assert(c.received_buf[0] == 0b10011001);
  });
  
  test("code_sender", []() {
    uint8_t msg[]{ 0b10011001 };
    CodeSender c(msg, 10, 1, 990);

    assert(!c.value_to_write_at(5));
    assert(!c.value_to_write_at(985));

    assert(!c.value_to_write_at(995));
    assert(c.value_to_write_at(1005));
    assert(c.value_to_write_at(1015));
    assert(!c.value_to_write_at(1025));
    assert(c.value_to_write_at(1035));
    assert(!c.value_to_write_at(1045));
    assert(!c.value_to_write_at(1055));
    assert(c.value_to_write_at(1065));
    assert(c.value_to_write_at(1075));
    assert(!c.value_to_write_at(1085));
    assert(c.value_to_write_at(1095));
    assert(!c.value_to_write_at(1105));
    assert(!c.value_to_write_at(1115));
    
    assert(c.value_to_write_at(1125));
    assert(c.value_to_write_at(1135));
    assert(!c.value_to_write_at(1145));
    assert(c.value_to_write_at(1155));
    assert(!c.value_to_write_at(1165));
    assert(c.value_to_write_at(1175));
    assert(c.value_to_write_at(1185));
    assert(!c.value_to_write_at(1195));
    assert(!c.value_to_write_at(1205));
    assert(c.value_to_write_at(1215));
    assert(!c.value_to_write_at(1225));
    assert(!c.value_to_write_at(1235));
    assert(c.value_to_write_at(1245));
    
    assert(c.value_to_write_at(123500));
  });

  test("code_sender_to_receiver", []() {
    uint8_t msg[]{ 1, 2, 3, 4, 90 };
    CodeSender s(msg, 10, sizeof(msg), 200);
    CodeReceiver r(10, sizeof(msg));

    for (unsigned long t = 0; t < 20 * sizeof(msg) * 8 + 200; t++) {
      bool b = s.value_to_write_at(t);
      r.did_read_at(b, t);
    }

    assert(r.did_receive);
    assert(r.received_buf[0] == msg[0]);
    assert(r.received_buf[1] == msg[1]);
    assert(r.received_buf[2] == msg[2]);
    assert(r.received_buf[3] == msg[3]);
    assert(r.received_buf[4] == msg[4]);
  });

  printf("CODER PASS\n");
}
