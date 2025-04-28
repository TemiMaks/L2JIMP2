import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.util.*;
import java.util.List;
import java.util.regex.*;

/**
 * A JPanel for visualizing a graph with nodes and edges,
 * either unpartitioned (grid layout) or coloured by partition (rows layout).
 */
public class GraphPanel extends JPanel {
    private enum Mode {RAW, PARTITIONED}

    private Mode mode;
    private final Map<Integer, Set<Integer>> adjacency = new HashMap<>();   //which nodes are connected to which
    private final Map<Integer, Integer> partitions = new HashMap<>();   // partition (group) each node belongs to
    private final Map<Integer, Point> positions = new HashMap<>();  //where to draw each node on the panel
    private final Color[] colors = generateColors(10);  // color different partitions when drawing the graph

    //Overload functions
    public void loadGraph(String grafFile) {
        mode = Mode.RAW;
        adjacency.clear();
        partitions.clear();
        positions.clear();
        parseEdges(grafFile);
        calculatePositions();
        repaint();
    }

    public void loadGraph(String podzialFile, String grafFile) {
        mode = Mode.PARTITIONED;
        adjacency.clear();
        partitions.clear();
        positions.clear();
        parsePodzial(podzialFile);
        parseEdges(grafFile);
        calculatePositions();
        repaint();
    }

    private void parseEdges(String filename) {
        Pattern p = Pattern.compile("(\\d+)\\s*-\\s*(\\d+)");   //number - number
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                Matcher m = p.matcher(line.trim());
                if (m.matches()) {
                    int u = Integer.parseInt(m.group(1));
                    int v = Integer.parseInt(m.group(2));
                    addEdge(u, v);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void addEdge(int u, int v) {
        adjacency.computeIfAbsent(u, k -> new HashSet<>()).add(v);
        adjacency.computeIfAbsent(v, k -> new HashSet<>()).add(u);
    }

    private void parsePodzial(String filename) {
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            int curPart = -1;
            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (line.startsWith("Partition")) {
                    curPart++;
                    int c = line.indexOf(':');
                    if (c >= 0) {
                        for (String tok : line.substring(c + 1).trim().split("\\s+")) {
                            if (!tok.isEmpty()) {
                                partitions.put(Integer.parseInt(tok), curPart); //adds each node to the partitions map with curPart as its value
                            }
                        }
                    }
                } else if (line.startsWith("Edges")) {
                    break;
                } else if (!line.isEmpty()) {
                    for (String tok : line.split("\\s+")) {
                        if (!tok.isEmpty()) {
                            partitions.put(Integer.parseInt(tok), curPart);
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void calculatePositions() {
        // if no nodes, nothing to do
        if (adjacency.isEmpty()) return;

        positions.clear();
        if (mode == Mode.RAW) {
            layoutGrid();
        } else {
            layoutPartitions();
        }
    }

    private void layoutGrid() {
        int n = adjacency.size();
        if (n == 0) return;

        int cols = (int) Math.ceil(Math.sqrt(n));
        int rows = (int) Math.ceil(n / (double) cols);
        int w = getWidth(), h = getHeight(), margin = 40;
        int cellW = (w - 2 * margin) / cols;
        int cellH = (h - 2 * margin) / rows;

        int i = 0;
        for (int node : adjacency.keySet()) {
            int row = i / cols, col = i % cols;

            // base center‐of‐cell
            int x = margin + col * cellW + cellW / 2;
            int y = margin + row * cellH + cellH / 2;

            // stagger even rows by half a cell
            if (row % 2 == 0) {
                x += cellW / 2;
            }

            positions.put(node, new Point(x, y));
            i++;
        }
    }


    private void layoutPartitions() {
        // group by partition
        Map<Integer, List<Integer>> byPart = new TreeMap<>();
        for (int node : adjacency.keySet()) {
            int p = partitions.getOrDefault(node, 0);
            byPart.computeIfAbsent(p, k -> new ArrayList<>()).add(node);
        }
        int parts = byPart.size();
        if (parts == 0) return;

        int w = getWidth(), h = getHeight(), margin = 40;
        int bandH = (h - 2 * margin) / parts;

        int row = 0;
        for (List<Integer> group : byPart.values()) {
            int m = group.size();
            if (m == 0) {
                row++;
                continue;
            }
            int cellW = (w - 2 * margin) / (m + 1);
            for (int j = 0; j < m; j++) {
                int node = group.get(j);
                int x = margin + (j + 1) * cellW;
                int y = margin + row * bandH + bandH / 2;
                positions.put(node, new Point(x, y));
            }
            row++;
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        calculatePositions();
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        // edges
        g2.setColor(Color.LIGHT_GRAY);
        for (var e : adjacency.entrySet()) {
            int u = e.getKey();
            for (int v : e.getValue()) {
                if (u < v) {
                    Point p1 = positions.get(u), p2 = positions.get(v);
                    g2.drawLine(p1.x, p1.y, p2.x, p2.y);
                }
            }
        }

        // nodes
        int r = 12;
        for (int node : positions.keySet()) {
            Point p = positions.get(node);
            int part = partitions.getOrDefault(node, 0);
            g2.setColor(colors[part % colors.length]);
            g2.fillOval(p.x - r, p.y - r, 2 * r, 2 * r);
            g2.setColor(Color.BLACK);
            g2.drawOval(p.x - r, p.y - r, 2 * r, 2 * r);
            g2.drawString(String.valueOf(node), p.x - 6, p.y + 4);
        }
    }

    private Color[] generateColors(int k) {
        Color[] cols = new Color[k];
        for (int i = 0; i < k; i++) {
            cols[i] = Color.getHSBColor(i / (float) k, 0.7f, 0.9f);
        }
        return cols;
    }
}


