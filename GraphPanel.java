import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.util.*;

/**
 * A JPanel for visualizing a graph with nodes and edges, colored by partitions.
 */
public class GraphPanel extends JPanel {
    private final Map<Integer, Set<Integer>> adjacency = new HashMap<>();
    private final Map<Integer, Integer> partitions = new HashMap<>();
    private final Map<Integer, Point> positions = new HashMap<>();
    private final Color[] colors = generateColors(100);

    public void loadGraph(String grafFile, String podzialFile) {
        adjacency.clear();
        partitions.clear();
        positions.clear();
        parseGraf(grafFile);
        parsePodzial(podzialFile);
        calculatePositions();
        repaint();
    }

    private void parseGraf(String filename) {
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            int row = 0;
            String line;
            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (!line.startsWith("[") || !line.endsWith("]")) continue;
                line = line.substring(1, line.length() - 1).trim();
                String[] tokens = line.split("\\s+");
                for (int col = 0; col < tokens.length; col++) {
                    if (tokens[col].equals("1.") || tokens[col].equals("1")) {
                        adjacency.computeIfAbsent(row, k -> new HashSet<>()).add(col);
                        adjacency.computeIfAbsent(col, k -> new HashSet<>()).add(row); // symmetric
                    }
                }
                row++;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void parsePodzial(String filename) {
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            int currentPartition = -1;
            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;
                if (line.startsWith("Partition")) {
                    currentPartition++;
                } else {
                    // Skip lines that aren't pure numbers
                    String[] tokens = line.trim().split("\\s+");
                    for (String token : tokens) {
                        try {
                            int node = Integer.parseInt(token);
                            partitions.put(node, currentPartition);
                        } catch (NumberFormatException ignored) {
                            // skip non-numeric entries like "Edges:"
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void calculatePositions() {
        int n = adjacency.keySet().size();
        int radius = Math.min(getWidth(), getHeight()) / 2 - 60;
        int cx = getWidth() / 2;
        int cy = getHeight() / 2;
        double angleStep = 2 * Math.PI / n;
        int i = 0;
        for (int node : adjacency.keySet()) {
            int x = (int) (cx + radius * Math.cos(i * angleStep));
            int y = (int) (cy + radius * Math.sin(i * angleStep));
            positions.put(node, new Point(x, y));
            i++;
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        calculatePositions();
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        // Draw edges
        g2.setColor(Color.LIGHT_GRAY);
        for (var entry : adjacency.entrySet()) {
            int from = entry.getKey();
            for (int to : entry.getValue()) {
                if (from < to) {
                    Point p1 = positions.get(from);
                    Point p2 = positions.get(to);
                    g2.drawLine(p1.x, p1.y, p2.x, p2.y);
                }
            }
        }

        // Draw nodes
        int radius = 12;
        for (int node : positions.keySet()) {
            Point p = positions.get(node);
            int partition = partitions.getOrDefault(node, 0);
            g2.setColor(colors[partition % colors.length]);
            g2.fillOval(p.x - radius, p.y - radius, radius * 2, radius * 2);
            g2.setColor(Color.BLACK);
            g2.drawOval(p.x - radius, p.y - radius, radius * 2, radius * 2);
            g2.drawString(String.valueOf(node), p.x - 6, p.y + 4);
        }
    }

    private Color[] generateColors(int n) {
        Color[] colors = new Color[n];
        Random rand = new Random(42);
        for (int i = 0; i < n; i++) {
            colors[i] = new Color(rand.nextFloat(), rand.nextFloat(), rand.nextFloat());
        }
        return colors;
    }
}