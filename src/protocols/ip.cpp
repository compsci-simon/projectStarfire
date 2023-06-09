#include <iostream>
#include <string>
#include "ip.h"
#include "icmp.h"
#include "datagram.h"

IP::IP() {
  memset(data, 0, IP_PAYLOAD_SIZE);
}

void IP::set_destination(const char* address) {
  destination_address = 0;
  int p1 = 0;
  int p2 = 0;
  int count = 0;
  int octet = 3;
  char temp_str[4];

  for (int i = 0; i < 15 && address[i] != '\0'; i++) {
    if (address[i] == '.') count++;
  }
  if (count != 3) {
    std::cerr << "Error decoding address. 3 octects are required. Found " << count << std::endl;
    return;
  }
  for (; octet > 0; octet--) {
    for (; p2 < 15 && address[p2] != '\0'; p2++) {
      temp_str[p2 - p1] = (char) address[p2];
      if (address[p2] == '.') {
        temp_str[1 + p2 - p1] = '\0';
        break;
      }
    }
    p2 = p1;
    destination_address = destination_address << octet*8;
    destination_address = destination_address & atoi(temp_str);
  }
  for (; p2 - p1 < 4 && address[p2] != '\0'; p2++) {
    temp_str[p2 - p1] = address[p2];
  }
  destination_address = destination_address << octet*8;
  destination_address = destination_address & atoi(temp_str);
  std::cout << destination_address << std::endl;
}

void IP::set_destination(int address) {
  destination_address = address;
}

void IP::set_source(int address) {
  source_address = address;
}

/*
This function is used to set the payload of an IP packet.
Parameters:
  buffer - A 1476 byte buffer that will be copied to the packet's
    payload
*/
void IP::set_payload(unsigned char* buffer) {
  memset(data, 0, IP_PAYLOAD_SIZE);
  memcpy(data, buffer, IP_PAYLOAD_SIZE);
}

/*
This method encapsulates a datagram.
Parameters:
  datagram - The datagram to encapsulate.
*/
void IP::set_payload(Datagram datagram) {
  datagram.get_bytestring(data);
}

/*
This function is used for transferring the payload of the
packet into a buffer that is passed in.
Parameters:
  buffer - This buffer should be exactly IP_PAYLOAD_SIZE bytes
*/
void IP::get_payload(unsigned char* buffer) {
  memset(buffer, 0, IP_PAYLOAD_SIZE);
  memcpy(buffer, data, IP_PAYLOAD_SIZE);
}

/*
This function copies a byte string representation of this packet into a buffer.
Parameters:
  buffer - A buffer that is of size IP_SIZE that will hold all the information.
*/
void IP::to_byte_string(unsigned char* buffer) {
  memset(buffer, 0, IP_SIZE);
  memcpy(buffer, &V_IHL, 1);
  buffer += 1;
  memcpy(buffer, &TOS, 1);
  buffer += 1;
  memcpy(buffer, &total_length, 2);
  buffer += 2;
  memcpy(buffer, &identification, 2);
  buffer += 2;
  memcpy(buffer, &flags_and_offset, 2);
  buffer += 2;
  memcpy(buffer, &TTL, 1);
  buffer += 1;
  memcpy(buffer, &protocol, 1);
  buffer += 1;
  memcpy(buffer, &header_checksum, 2);
  buffer += 2;
  memcpy(buffer, &source_address, 4);
  buffer += 4;
  memcpy(buffer, &destination_address, 4);
  buffer += 4;
  memcpy(buffer, &options, 8);
  buffer += 8;
  memcpy(buffer, data, IP_PAYLOAD_SIZE);
}

void IP::load_packet_from_byte_string(unsigned char* byte_string) {
  memcpy(&V_IHL, byte_string, 1);
  byte_string += 1;
  memcpy(&TOS, byte_string, 1);
  byte_string += 1;
  memcpy(&total_length, byte_string, 2);
  byte_string += 2;
  memcpy(&identification, byte_string, 2);
  byte_string += 2;
  memcpy(&flags_and_offset, byte_string, 2);
  byte_string += 2;
  memcpy(&TTL, byte_string, 1);
  byte_string += 1;
  memcpy(&protocol, byte_string, 1);
  byte_string += 1;
  memcpy(&header_checksum, byte_string, 2);
  byte_string += 2;
  memcpy(&source_address, byte_string, 4);
  byte_string += 4;
  memcpy(&destination_address, byte_string, 4);
  byte_string += 4;
  memcpy(&options, byte_string, 8);
  byte_string += 8;
  memcpy(data, byte_string, IP_PAYLOAD_SIZE);
}

/*
This method is used to instantiate a datagram from the bytestring that
is the payload of an IP packet.
Parameters:
  datagram - A pointer to the datagram to instantiate.
*/
void IP::load_datagram(Datagram* datagram) {
  datagram->instantiate_from_bytestring(data);
}

char* IP::address_to_string(bool source) {
  memset(address_string, 0, 17);
  if (source) {
    IP::address_to_string(this->get_source(), address_string);
  } else {
    IP::address_to_string(this->get_destination(), address_string);
  }
  return address_string;
}

/*
This method is used to get a string representation
of a packet's address
*/
void IP::address_to_string(int address, char* buffer) {
  int pos = 0;
  int octetVal = 0;
  memset(buffer, 0, 17);
  
  for (int octet = 0; octet < 4; octet++) {
    octetVal = (address >> ((3-octet)*8)) & 0xff;
    if (octet != 0) {
      buffer[pos++] = '.';
    }
    if (octetVal > 99) {
      buffer[pos++] = (unsigned char) (octetVal / 100 + '0');
      buffer[pos++] = (unsigned char) ((octetVal % 100) / 10 + '0');
      buffer[pos++] = (unsigned char) (octetVal % 10 + '0');
    } else if (octetVal > 9) {
      buffer[pos++] = (unsigned char) ((octetVal % 100) / 10 + '0');
      buffer[pos++] = (unsigned char) (octetVal % 10 + '0');
    } else {
      buffer[pos++] = (unsigned char) (octetVal % 10 + '0');
    }
  }
}

void IP::encapsulate(ICMP message) {
  memset(data, 0, 1472);
  message.get_byte_string(data);
}

void IP::unencapsulate(ICMP* message) {
  message->instantiate_from_byte_string(data);
}