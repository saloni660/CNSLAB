import java.util.Scanner;

public class SubnetCalculator {

    public static String cidrToSubnetMask(int cidr) {
        int mask = 0xffffffff << (32 - cidr);
        int octet1 = (mask >>> 24) & 0xff;
        int octet2 = (mask >>> 16) & 0xff;
        int octet3 = (mask >>> 8) & 0xff;
        int octet4 = mask & 0xff;
        return octet1 + "." + octet2 + "." + octet3 + "." + octet4;
    }

    public static long ipToLong(String ip) {
        String[] octets = ip.split("\\.");
        long result = 0;
        for (int i = 0; i < 4; i++) {
            result |= (long) Integer.parseInt(octets[i]) << (24 - (8 * i));
        }
        return result;
    }

    public static String longToIp(long ip) {
        return ((ip >> 24) & 0xff) + "." +
               ((ip >> 16) & 0xff) + "." +
               ((ip >> 8) & 0xff) + "." +
               (ip & 0xff);
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        // Use IP based on your roll number
        String baseIp = "192.168.50.0";
        int cidr = 26;  // You can also take user input here

        System.out.println("Base IP (from roll no): " + baseIp);
        System.out.println("CIDR Notation: /" + cidr);

        String subnetMask = cidrToSubnetMask(cidr);
        System.out.println("Subnet Mask: " + subnetMask);

        int totalHosts = (int) Math.pow(2, 32 - cidr);
        int usableHosts = totalHosts - 2;

        System.out.println("Total IPs in subnet: " + totalHosts);
        System.out.println("Usable Hosts per Subnet: " + usableHosts);

        // Calculate subnet ranges
        long baseIpLong = ipToLong(baseIp);
        int subnetCount = (int) Math.pow(2, cidr - 24);  // if subnetting from /24
        int hostsPerSubnet = totalHosts;

        System.out.println("\nSubnets:");
        for (int i = 0; i < subnetCount; i++) {
            long subnetStart = baseIpLong + (i * hostsPerSubnet);
            long subnetEnd = subnetStart + hostsPerSubnet - 1;

            String firstIp = longToIp(subnetStart + 1);
            String lastIp = longToIp(subnetEnd - 1);
            String network = longToIp(subnetStart);
            String broadcast = longToIp(subnetEnd);

            System.out.println("Subnet " + (i + 1) + ":");
            System.out.println("  Network Address: " + network);
            System.out.println("  First Host:      " + firstIp);
            System.out.println("  Last Host:       " + lastIp);
            System.out.println("  Broadcast:       " + broadcast);
            System.out.println();
        }

        scanner.close();
    }
}
