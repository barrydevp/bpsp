package com.ui;

import com.core.client.BPSPClient;
import com.core.client.Publisher;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.swing.*;
import java.awt.*;

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
        static final int MAX_HEIGHT = 380;

        // UI
        GridBagConstraints c = new GridBagConstraints();
        DefaultListModel<GeneratorPublisher> generatorModel = new DefaultListModel<>();
        JList<GeneratorPublisher> generators = new JList<>(generatorModel);
        JScrollPane listPanel = new JScrollPane(generators);
        JLabel topicLb = new JLabel("Topic");
        JTextField topicField = new JTextField();
        JLabel typeLb = new JLabel("Type");
        JComboBox<DevicePanel.DeviceType> typeField = new JComboBox<>(DevicePanel.DeviceType.values());
        //        JLabel msgLb = new JLabel("Message");
//        JTextField msgField = new JTextField();
        JButton addBtn = new JButton("Add");
        JButton delBtn = new JButton("Delete");
        JButton clearLogBtn = new JButton("Clear log");
        DefaultListModel<String> logModel = new DefaultListModel<>();
        JList<String> logs = new JList<>(logModel);
        JScrollPane logPanel = new JScrollPane(logs);

        // data
        boolean running = false;
        Thread loop;

        public GeneratorList() {
            this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, MAX_HEIGHT));
            this.setSize(this.getPreferredSize());
            this.setLayout(new GridBagLayout());
            this.setBorder(BorderFactory.createTitledBorder(
                    BorderFactory.createLineBorder(Color.PINK, 1, true), "Generator List")
            );

            c.gridx = 0;
            c.gridy = 0;
            c.weightx = 1;
            c.weighty = 0.05;
            c.fill = GridBagConstraints.HORIZONTAL;
            this.add(topicLb, c);

            c.gridx = 1;
            c.gridy = 0;
            topicField.setColumns(10);
            this.add(topicField, c);

            c.gridx = 0;
            c.gridy = 1;
            this.add(typeLb, c);

            c.gridx = 1;
            c.gridy = 1;
            this.add(typeField, c);

            c.gridx = 0;
            c.gridy = 2;
            addBtn.addActionListener((event) -> {
                if (topicField.getText().isEmpty() || typeField.getSelectedItem() == null) {
                    return;
                }

                this.generatorModel.addElement(
                        new GeneratorPublisher(topicField.getText(),
                                Generator.getGenerator((DevicePanel.DeviceType) typeField.getSelectedItem()))
                );

            });
            this.add(addBtn, c);

            c.gridx = 1;
            c.gridy = 2;
            delBtn.addActionListener((event) -> {
                if (typeField.getSelectedItem() == null) {
                    return;
                }

                this.generatorModel.removeElementAt(typeField.getSelectedIndex());
            });
            this.add(delBtn, c);

//            c.gridx = 0;
//            c.gridy = 3;
//            clearLogBtn.addActionListener((event) -> {
//                this.logModel.clear();
//            });
//            this.add(clearLogBtn, c);
//
            c.gridx = 0;
            c.gridy = 3;
            c.weighty = 1;
            c.gridwidth = 2;
            c.fill = GridBagConstraints.BOTH;

//            listPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "LIST"));
            this.add(listPanel, c);

        }

        public void startLoop() {
            if (this.loop != null) {
                return;
            }

            this.running = true;
            loop = new Thread(() -> {
                try {
                    while (running) {
                        for (Object obj : GeneratorList.this.generatorModel.toArray()) {
                            GeneratorPublisher publisher = (GeneratorPublisher) obj;
                            publisher.pub();
                        }
                        Thread.sleep(2000);
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

        public GeneratorPublisher(String topic, Generator gen) {
            super(GeneratorUI.this.getBPSPClient(), topic, "");
            this.gen = gen;
        }

        public void pub() {
            super.setMsg(gen.getData());
            try {
                this.client.pub(this);
            } catch (Exception e) {
                LOGGER.error(e);
            }
        }
    }
}
