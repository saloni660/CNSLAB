import java.util.*;

class Router {
    int id;
    Map<Integer, Integer> distanceVector = new HashMap<>();
    Map<Integer, Integer> neighbors = new HashMap<>();

    public Router(int id, int totalRouters) {
        this.id = id;
        // Initialize distance vector
        for (int i = 0; i < totalRouters; i++) {
            distanceVector.put(i, i == id ? 0 : Integer.MAX_VALUE);
        }
    }

    public void addNeighbor(int neighborId, int cost) {
        neighbors.put(neighborId, cost);
    }

    public boolean update(Map<Integer, Router> allRouters) {
        boolean updated = false;

        for (Map.Entry<Integer, Integer> neighborEntry : neighbors.entrySet()) {
            int neighborId = neighborEntry.getKey();
            int costToNeighbor = neighborEntry.getValue();
            Router neighbor = allRouters.get(neighborId);

            for (Map.Entry<Integer, Integer> destEntry : neighbor.distanceVector.entrySet()) {
                int dest = destEntry.getKey();
                int newCost = costToNeighbor + destEntry.getValue();

                if (newCost < distanceVector.getOrDefault(dest, Integer.MAX_VALUE)) {
                    distanceVector.put(dest, newCost);
                    updated = true;
                }
            }
        }

        return updated;
    }

    public void printRoutingTable() {
        System.out.println("Routing table for Router " + id + ":");
        for (Map.Entry<Integer, Integer> entry : distanceVector.entrySet()) {
            System.out.printf("Destination: %d, Cost: %s\n", entry.getKey(),
                    entry.getValue() == Integer.MAX_VALUE ? "∞" : entry.getValue());
        }
        System.out.println();
    }
}

public class DistanceVectorRouting {
    public static void main(String[] args) {
        int totalRouters = 4;

        Map<Integer, Router> routers = new HashMap<>();

        // Initialize routers
        for (int i = 0; i < totalRouters; i++) {
            routers.put(i, new Router(i, totalRouters));
        }

        // Define the network topology
        // routerA.addNeighbor(routerB, cost)
        routers.get(0).addNeighbor(1, 1);
        routers.get(0).addNeighbor(2, 4);

        routers.get(1).addNeighbor(0, 1);
        routers.get(1).addNeighbor(2, 2);
        routers.get(1).addNeighbor(3, 6);

        routers.get(2).addNeighbor(0, 4);
        routers.get(2).addNeighbor(1, 2);
        routers.get(2).addNeighbor(3, 3);

        routers.get(3).addNeighbor(1, 6);
        routers.get(3).addNeighbor(2, 3);

        // Distance Vector algorithm iteration
        boolean updated;
        int iterations = 0;
        do {
            updated = false;
            iterations++;
            for (Router router : routers.values()) {
                boolean changed = router.update(routers);
                updated = updated || changed;
            }
        } while (updated && iterations < 10); // prevent infinite loops

        // Display final routing tables
        for (Router router : routers.values()) {
            router.printRoutingTable();
        }
    }
}
