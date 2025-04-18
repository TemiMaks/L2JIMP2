import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;

/**
 * GUI for the standalone graphPartitioner_cli.exe tool, with added graph visualization.
 */
public class PartitionToolGUI {
    private static final String EXECUTABLE = "bin/graphPartitioner_cli.exe";

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Graph Partitioner");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setSize(1000, 700);

            JPanel main = new JPanel(new BorderLayout(10, 10));
            main.setBorder(new EmptyBorder(10, 10, 10, 10));
            frame.setContentPane(main);

            // Top controls
            JPanel controls = new JPanel(new FlowLayout(FlowLayout.LEFT));
            JTextField fileField = new JTextField(40);
            fileField.setEditable(false);
            JButton browse = new JButton("Browse...");
            JButton process = new JButton("Process");
            process.setEnabled(false);

            SpinnerNumberModel partsModel = new SpinnerNumberModel(3, 1, 100, 1);
            JSpinner partsSpinner = new JSpinner(partsModel);
            partsSpinner.setPreferredSize(new Dimension(60, partsSpinner.getPreferredSize().height));

            SpinnerNumberModel thresholdModel = new SpinnerNumberModel(0.05, 0.01, 1.0, 0.01);
            JSpinner thresholdSpinner = new JSpinner(thresholdModel);
            thresholdSpinner.setPreferredSize(new Dimension(60, thresholdSpinner.getPreferredSize().height));

            controls.add(new JLabel("Partitions:"));
            controls.add(partsSpinner);
            controls.add(new JLabel("Threshold:"));
            controls.add(thresholdSpinner);
            controls.add(fileField);
            controls.add(browse);
            controls.add(process);
            main.add(controls, BorderLayout.NORTH);

            // Tabs for output
            JTabbedPane tabs = new JTabbedPane();
            JTextArea matrixArea = new JTextArea();
            JTextArea partitionArea = new JTextArea();
            for (JTextArea ta : new JTextArea[]{matrixArea, partitionArea}) {
                ta.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
                ta.setEditable(false);
            }

            tabs.addTab("Adjacency (graf.txt)", new JScrollPane(matrixArea));
            tabs.addTab("Partition (podzial.txt)", new JScrollPane(partitionArea));
            GraphPanel graphPanel = new GraphPanel();
            tabs.addTab("Graph View", new JScrollPane(graphPanel));
            main.add(tabs, BorderLayout.CENTER);

            // File selection
            browse.addActionListener(e -> {
                JFileChooser chooser = new JFileChooser(new File(System.getProperty("user.home"), "Downloads"));
                chooser.setDialogTitle("Select a .csrrg graph file");
                chooser.setAcceptAllFileFilterUsed(false);
                chooser.setFileFilter(new FileNameExtensionFilter("CSRRG Graph Files (*.csrrg)", "csrrg"));
                if (chooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) {
                    fileField.setText(chooser.getSelectedFile().getAbsolutePath());
                    process.setEnabled(true);
                }
            });

            // Processing
            process.addActionListener((ActionEvent e) -> {
                String filePath = fileField.getText();
                int numParts = (Integer) partsSpinner.getValue();
                double threshold = (Double) thresholdSpinner.getValue();
                runPartitioner(filePath, numParts, threshold, frame, matrixArea, partitionArea, graphPanel);
            });

            frame.setVisible(true);
        });
    }

    private static void runPartitioner(String cssrgPath, int numParts, double threshold, JFrame frame,
                                       JTextArea matrixArea, JTextArea partitionArea, GraphPanel graphPanel) {
        try {
            //Use .exe from C project to call the main method (main_partition.c, with given parameters)
            ProcessBuilder pb = new ProcessBuilder(EXECUTABLE, cssrgPath, String.valueOf(numParts), String.valueOf(threshold));
            pb.redirectErrorStream(true);
            pb.directory(new File(System.getProperty("user.dir")));
            Process p = pb.start();

            try (BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()))) {
                String ln;
                while ((ln = br.readLine()) != null) System.out.println(ln);
            }

            int rc = p.waitFor();
            if (rc != 0) {
                JOptionPane.showMessageDialog(frame, "Partitioner failed (exit code=" + rc + ")", "Error", JOptionPane.ERROR_MESSAGE);
                return;
            }

            matrixArea.setText(Files.readString(Paths.get("graf.txt")));
            partitionArea.setText(Files.readString(Paths.get("podzial.txt")));
            graphPanel.loadGraph("graf.txt", "podzial.txt");

        } catch (IOException | InterruptedException ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog(frame, "Exception: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }
}