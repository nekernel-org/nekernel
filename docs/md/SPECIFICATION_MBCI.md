===================================

# 0: General Information

===================================

- High Speed Bus Format.
- Designed for keyboards, mouses, interfaces.
- A open and cheap alternative to other HCIs.

===================================

# 1: Concepts

===================================

- MBCI Host
- MBCI Authentication key (24-bit number)
- MBCI Host Kind and Flags.

===================================

# 2: Pinout

===================================

- VCC (IN) (OUT for MCU)
- CLK (IN) (OUT for MCU)
- ACK (BI) (Contains an Acknowledge Packet Frame)
- D0- (IN) (Starts with the Host Interface Packet Frame)
- D1- (IN) (Starts with the Host Interface Packet Frame)
- D0+ (OUT) (Starts with the Host Interface Packet Frame)
- D1+ (OUT) (Starts with the Host Interface Packet Frame)
- GND (IN) (OUT for MCU)