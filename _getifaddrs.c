
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


#define ENV_ROS_IP "ROS_IP"

#define IFADDRS_T struct ifaddrs
#define AS_ADDR4_IN(x) ((struct sockaddr_in*)x)

#define NAME_LO "lo"
#define NAME_ETH0 "eth0"
#define FLAGS_LO (IFF_UP | IFF_LOOPBACK | IFF_RUNNING)
#define FLAGS_ETH0 (IFF_UP | IFF_BROADCAST | IFF_RUNNING | IFF_MULTICAST)
#define ADDR_LO "127.0.0.1"
#define ADDR_ETH0 "192.168.0.2"
#define MASK_LO "255.0.0.0"
#define MASK_ETH0 "255.255.255.0"


int setup_ifaddr(IFADDRS_T* ifap, const char* ifname, unsigned int flags, const char* ifaddr, const char* netmask)
{
  if (ifap == NULL)
    return -1;

  bzero(ifap, sizeof(IFADDRS_T));

  ifap->ifa_addr = (struct sockaddr*) malloc(sizeof(struct sockaddr));
  ifap->ifa_netmask = (struct sockaddr*) malloc(sizeof(struct sockaddr));

  bzero(ifap->ifa_addr, sizeof(struct sockaddr));
  bzero(ifap->ifa_netmask, sizeof(struct sockaddr));

  ifap->ifa_name = (char*) malloc(16 * sizeof(char));
  strncpy(ifap->ifa_name, ifname, 16);

  ifap->ifa_flags = flags;

  // ip
  AS_ADDR4_IN(ifap->ifa_addr)->sin_family = AF_INET;
  inet_aton(ifaddr, &AS_ADDR4_IN(ifap->ifa_addr)->sin_addr);

  // netmask
  AS_ADDR4_IN(ifap->ifa_netmask)->sin_family = AF_INET;
  inet_aton(netmask, &AS_ADDR4_IN(ifap->ifa_netmask)->sin_addr);

  // broadcast addr (no p-to-p)
  if (ifap->ifa_flags & IFF_BROADCAST)
  {
    ifap->ifa_broadaddr = (struct sockaddr*) malloc(sizeof(struct sockaddr));
    bzero(ifap->ifa_broadaddr, sizeof(struct sockaddr));
    AS_ADDR4_IN(ifap->ifa_broadaddr)->sin_family = AF_INET;

    // bcast = ip | ~mask
    AS_ADDR4_IN(ifap->ifa_broadaddr)->sin_addr.s_addr = (
      AS_ADDR4_IN(ifap->ifa_addr)->sin_addr.s_addr |
        ~AS_ADDR4_IN(ifap->ifa_netmask)->sin_addr.s_addr);
  }

  return 0;
}


int getifaddrs(IFADDRS_T **ifap)
{
  int res = 0;

  IFADDRS_T* ifa_lo = (IFADDRS_T *) malloc(sizeof(IFADDRS_T));
  if ((res = setup_ifaddr(ifa_lo, NAME_LO, FLAGS_LO, ADDR_LO, MASK_LO)) < 0)
    return -1;

  // use ROS_IP if set
  char* eth0_addr = NULL;
  if ((eth0_addr = getenv(ENV_ROS_IP)) == NULL)
    eth0_addr = ADDR_ETH0;

  IFADDRS_T* ifa_eth0 = (IFADDRS_T *) malloc(sizeof(IFADDRS_T));
  if ((res = setup_ifaddr(ifa_eth0, NAME_ETH0, FLAGS_ETH0, eth0_addr, MASK_ETH0)) < 0)
    // TODO: leaks
    return -1;

  *ifap = (IFADDRS_T*) malloc(sizeof(IFADDRS_T*) * 2);
  ifap[0] = ifa_lo;
  ifap[1] = ifa_eth0;

  ifa_lo->ifa_next = ifa_eth0;

  return 0;
}
