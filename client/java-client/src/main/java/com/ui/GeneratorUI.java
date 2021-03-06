package com.ui;

import com.core.client.BPSPClient;
import com.core.client.Publisher;
import com.core.frame.Frame;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.time.Instant;
import java.util.Vector;

public class GeneratorUI extends AbstractUI {
    private static final Logger LOGGER = LogManager.getLogger(GeneratorUI.class);

    // UI
    HeaderPanel headerPanel = new HeaderPanel("SMART HOME - Generator BOT UI");
    ConnectPanel connectPanel = new ConnectPanel(this);
    GeneratorList generatorList = new GeneratorList();

    public GeneratorUI() {
        super("Generator BOT");

        this.setLayout(new FlowLayout());
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, AbstractUI.MAX_HEIGHT));
        this.setSize(this.getPreferredSize());

        Container c = this.getContentPane();
//        c.setBackground(Color.WHITE);

        c.add(headerPanel);
        c.add(connectPanel);
        c.add(generatorList);

        this.setLocationRelativeTo(null);
        this.pack();
        this.setVisible(true);
    }

    @Override
    public void onConnected() {
        generatorList.startLoop();
    }

    @Override
    public void onClosed() {
        generatorList.stopLoop();
    }

    public BPSPClient getBPSPClient() {
        return connectPanel.getClient();
    }

    class GeneratorList extends JPanel {
        static final int MAX_HEIGHT = 330;
        static final int MAX_LOGS = 100;
        final String[] columnNames = {
                "Topic",
                "Type",
                "Last Sent",
                "Publisher",
        };

        // UI
        GridBagConstraints c = new GridBagConstraints();
        JTabbedPane tabbedPane = new JTabbedPane();
        JPanel listContainer = new JPanel();
        DefaultTableModel generatorModel = new DefaultTableModel(columnNames, 0);
        JTable generators = new JTable(generatorModel) {
            private static final long serialVersionUID = 1L;

            public boolean isCellEditable(int row, int column) {
                return false;
            }
        };
        JScrollPane listPanel = new JScrollPane(generators);
        JLabel topicLb = new JLabel("Topic");
        JTextField topicField = new JTextField();
        JLabel typeLb = new JLabel("Type");
        JComboBox<DevicePanel.DeviceType> typeField = new JComboBox<>(DevicePanel.DeviceType.values());
        JButton addBtn = new JButton("Add");
        JButton delBtn = new JButton("Delete");
        JLabel intervalLb = new JLabel("Interval(ms)");
        JTextField intervalField = new JTextField();
        JButton updateIntervalBtn = new JButton("Update Interval");
        JButton startBtn = new JButton("Start Run");
        JButton stopBtn = new JButton("Stop Run");
        JButton clearLogBtn = new JButton("Clear log");
        DefaultListModel<String> logModel = new DefaultListModel<>();
        JList<String> logs = new JList<>(logModel);
        JPanel logContainer = new JPanel(new GridBagLayout());

        // data
        boolean running = false;
        int interval = 5000;
        Thread loop;

        public GeneratorList() {
            super(new GridLayout(1, 1));
//            this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, MAX_HEIGHT));
//            this.setSize(this.getPreferredSize());

            listContainer.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, MAX_HEIGHT));
            listContainer.setSize(this.getPreferredSize());
            listContainer.setLayout(new GridBagLayout());
            listContainer.setBorder(BorderFactory.createTitledBorder(
                    BorderFactory.createLineBorder(Color.PINK, 1, true), "Generator List")
            );

            c.gridx = 0;
            c.gridy = 0;
            c.weightx = 0.025;
            c.weighty = 0.05;
            c.fill = GridBagConstraints.HORIZONTAL;
            topicLb.setHorizontalAlignment(JLabel.CENTER);
            listContainer.add(topicLb, c);

            c.gridx = 1;
            c.gridy = 0;
            c.weightx = 0.15;
            topicField.setColumns(10);
            listContainer.add(topicField, c);

            c.gridx = 2;
            c.gridy = 0;
            c.weightx = 0.01;
            typeLb.setHorizontalAlignment(JLabel.CENTER);
            listContainer.add(typeLb, c);

            c.gridx = 3;
            c.gridy = 0;
            c.weightx = 0.1;
            listContainer.add(typeField, c);

            c.gridx = 4;
            c.gridy = 0;
            c.weightx = 0.01;
            addBtn.addActionListener((event) -> {
                if (topicField.getText().isEmpty() || typeField.getSelectedItem() == null) {
                    return;
                }

                GeneratorPublisher publisher = new GeneratorPublisher(topicField.getText(),
                        Generator.getGenerator((DevicePanel.DeviceType) typeField.getSelectedItem()));

                this.generatorModel.addRow(new Object[]{
                        topicField.getText(),
                        typeField.getSelectedItem(),
                        publisher.lastSent,
                        publisher
                });
//                new GeneratorPublisher(topicField.getText(),
//                        Generator.getGenerator((DevicePanel.DeviceType) typeField.getSelectedItem()))

            });
            listContainer.add(addBtn, c);

            c.gridx = 5;
            c.gridy = 0;
            delBtn.addActionListener((event) -> {
                if (generators.getSelectedRow() == -1) {
                    return;
                }

                this.generatorModel.removeRow(generators.getSelectedRow());
            });
            listContainer.add(delBtn, c);

            c.gridx = 0;
            c.gridy = 1;
            c.weightx = 0.025;
            intervalLb.setHorizontalAlignment(JLabel.CENTER);
            listContainer.add(intervalLb, c);

            c.gridx = 1;
            c.gridy = 1;
            c.weightx = 0.15;
            intervalField.setText(this.interval + "");
            intervalField.setColumns(10);
            listContainer.add(intervalField, c);

            c.gridx = 2;
            c.gridy = 1;
            c.weightx = 0.01;
            updateIntervalBtn.addActionListener((event) -> {
                try {
                    int newInterval = Integer.parseInt(this.intervalField.getText());
                    if (newInterval <= 200) {
                        throw new IllegalArgumentException("Interval must greater than 200ms");
                    }
                    this.interval = newInterval;
                } catch (Exception e) {
                    JOptionPane.showMessageDialog(
                            GeneratorUI.this,
                            e.getMessage(),
                            "Update Interval Error",
                            JOptionPane.ERROR_MESSAGE
                    );
                }
            });
            listContainer.add(updateIntervalBtn, c);

            c.gridx = 3;
            c.gridy = 1;
            startBtn.addActionListener((event) -> {
                if (!this.running) {
                    this.startLoop();
                }
            });
            listContainer.add(startBtn, c);

            c.gridx = 4;
            c.gridy = 1;
            stopBtn.addActionListener((event) -> {
                if (this.running) {
                    this.stopLoop();
                }
            });
            listContainer.add(stopBtn, c);

            c.gridx = 0;
            c.gridy = 2;
            c.weighty = 1;
            c.gridwidth = 8;
            c.fill = GridBagConstraints.BOTH;

//            listPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "LIST"));
            listContainer.add(listPanel, c);

            tabbedPane.addTab("List", null, listContainer, "List topic generator");
            tabbedPane.setForeground(Color.RED);

            logContainer.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, MAX_HEIGHT));
            logContainer.setSize(this.getPreferredSize());
            GridBagConstraints c1 = new GridBagConstraints();
            c1.gridx = 0;
            c1.gridy = 0;
            c1.weightx = 1;
            c1.weighty = 0.05;
//            c1.fill = GridBagConstraints.HORIZONTAL;
            clearLogBtn.addActionListener((event) -> {
                this.logModel.clear();
            });
            logContainer.add(clearLogBtn, c1);

            JScrollPane logScrollPanel = new JScrollPane(logs);
            logScrollPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "Logs"));
            logs.setOpaque(false);
            logScrollPanel.setOpaque(false);
            logScrollPanel.getViewport().setOpaque(false);
            c1.gridx = 0;
            c1.gridy = 1;
            c1.weighty = 0.5;
            c1.gridwidth = 1;
            c1.fill = GridBagConstraints.BOTH;
            logContainer.add(logScrollPanel, c1);

            tabbedPane.addTab("Logs", null, logContainer, "Logging");

            //Add the tabbed pane to this panel.
            this.add(tabbedPane, FlowLayout.LEFT);

            //The following line enables to use scrolling tabs.
            tabbedPane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
        }

        void addLog(String item) {
            logModel.addElement(item);
            if (logModel.size() > MAX_LOGS) {
                logModel.removeElementAt(0);
            }
            int lastIndex = logs.getModel().getSize() - 1;
            if (lastIndex >= 0) {
                logs.ensureIndexIsVisible(lastIndex);
            }
        }

        public void startLoop() {
            if (this.loop != null) {
                return;
            }

            this.running = true;
            loop = new Thread(() -> {
                LOGGER.info("Start publisher loop");
                try {
                    while (running) {
                        for (Vector vec : GeneratorList.this.generatorModel.getDataVector()) {
                            GeneratorPublisher publisher = (GeneratorPublisher) vec.get(3);
//                            GeneratorPublisher publisher = (GeneratorPublisher) obj;
                            publisher.pub();
                            vec.setElementAt(publisher.lastSent, 2);
//                            this.generatorModel.setValueAt(publisher.lastSent, );
                        }
                        GeneratorList.this.generatorModel.fireTableDataChanged();
                        Thread.sleep(this.interval);
                    }
                } catch (InterruptedException ex) {
                    LOGGER.error(ex);
                    running = false;
                }

                loop = null;
            });

            loop.start();
        }

        public void stopLoop() {
            if (this.loop == null) {
                return;
            }

            this.loop.interrupt();
            this.running = false;
        }
    }

    class GeneratorPublisher extends Publisher {

        Generator gen;
        Instant lastSent;

        public GeneratorPublisher(String topic, Generator gen) {
            super(GeneratorUI.this.getBPSPClient(), topic, "");
            this.gen = gen;
        }

        @Override
        public void replyOk(com.core.frame.Frame reply) {
            super.replyOk(reply);
            GeneratorUI.this.generatorList.addLog("OK \"" + this.getMsg() + "\" to \"" + this.getTopic() + "\": " + reply.getStrData());
        }

        @Override
        public void replyError(Frame reply) {
            super.replyError(reply);
            GeneratorUI.this.generatorList.addLog("ERR \"" + this.getMsg() + "\" to \"" + this.getTopic() + "\": " + reply.getStrData());
        }

        public void pub() {
            super.setMsg(gen.getData());
            try {
                this.client.pub(this);
                this.lastSent = Instant.now();
            } catch (Exception e) {
                LOGGER.error(e);
            }
        }
    }
}
