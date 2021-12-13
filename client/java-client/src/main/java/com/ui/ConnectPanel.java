package com.ui;

import com.core.client.BPSPClient;
import com.core.client.Publisher;
import com.core.frame.Frame;

import javax.swing.*;
import java.awt.*;

public class ConnectPanel extends JPanel {
    static final int BORDER_SIZE = 5;
    static final int HEIGHT = 120;

    AbstractUI mainUI;
    BPSPClient client = new BPSPClient();
    ConnectButton connectBtn = new ConnectButton();
    InputForm inputForm = new InputForm();
    boolean isConnected = false;
    JButton toolBtn = new JButton("Tool");
    ToolFrame toolFrame = new ToolFrame();

    public ConnectPanel(AbstractUI mainUI) {
        this.mainUI = mainUI;
        this.setLayout(new FlowLayout());
        this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, HEIGHT));
        this.setSize(this.getPreferredSize());
        this.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE),
                BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "Connector")
        ));
//        this.setBackground(Color.WHITE);

        this.add(inputForm, FlowLayout.LEFT);
        this.add(connectBtn);

        toolBtn.addActionListener((event) -> {
            this.toolFrame.setVisible(true);
        });

        this.add(toolBtn);
    }

    protected void connect() {
        // disable input
        connectBtn.setEnabled(false);
        inputForm.setInputEnabled(false);

        try {
            this._closeClient();

            client.connect(inputForm.getAddress(), inputForm.getPort());

            connectBtn.setText("Close");
            isConnected = true;
            client.startMainLoop();

            mainUI.onConnected();

        } catch (Exception e) {
            JOptionPane.showMessageDialog(
                    mainUI,
                    e.getMessage(),
                    "Connect Error",
                    JOptionPane.ERROR_MESSAGE
            );

            // enable for retry
            inputForm.setInputEnabled(true);
        }

        connectBtn.setEnabled(true);
    }

    private void _closeClient() {
        if (client == null) {
            client = new BPSPClient();
        } else if (client.isConnected()) {
            client.close();
        }
    }

    protected void close() {
        connectBtn.setEnabled(false);

        try {
            this._closeClient();

            connectBtn.setText("Connect");
            // enable for new connect
            inputForm.setInputEnabled(true);
            isConnected = false;

            mainUI.onClosed();

        } catch (Exception e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(
                    mainUI,
                    e.getMessage(),
                    "Close Error",
                    JOptionPane.ERROR_MESSAGE
            );
        }

        connectBtn.setEnabled(true);
    }

    public BPSPClient getClient() {
        return client;
    }

    public boolean isConnected() {
        return isConnected;
    }

    class InputForm extends JPanel {
        JLabel addressLabel = new JLabel("Address: ");
        JTextField addressField = new JTextField("192.168.0.129");
        JLabel portLabel = new JLabel("Port number: ");
        JTextField portField = new JTextField("29010");

        InputForm() {
            super(new GridLayout(2, 1, 0, 10));

            addressField.setColumns(10);
            portField.setColumns(10);

            this.add(addressLabel);
            this.add(addressField);
            this.add(portLabel);
            this.add(portField);
        }

        String getAddress() {
            return addressField.getText();
        }

        int getPort() {
            return Integer.parseInt(portField.getText());
        }

        public void setInputEnabled(boolean enabled) {
            addressField.setEnabled(enabled);
            portField.setEnabled(enabled);
        }
    }

    class ConnectButton extends JButton {
        public ConnectButton() {
            super("Connect");

            this.addActionListener(event -> {
                ConnectPanel connectPanel = ConnectPanel.this;

                if (!connectPanel.isConnected) {
                    connectPanel.connect();
                } else {
                    connectPanel.close();
                }
            });
        }
    }

    class ToolFrame extends JFrame {
        static final int MAX_WIDTH = 400;
        static final int MAX_HEIGHT = 800;

        JPanel panel = new JPanel();
        GridBagConstraints c = new GridBagConstraints();
        JLabel topicLb = new JLabel("Topic");
        JTextField topicField = new JTextField();
        JLabel msgLb = new JLabel("Message");
        JTextField msgField = new JTextField();
        JButton publishBtn = new JButton("Publish");
        JButton clearLogBtn = new JButton("Clear log");
        DefaultListModel<String> logModel = new DefaultListModel<>();
        JList<String> logs = new JList<>(logModel);
        JScrollPane logPanel = new JScrollPane(logs);

        public ToolFrame() {
            super("Publish Tool");

            panel.setPreferredSize(new Dimension(MAX_WIDTH, MAX_HEIGHT));
            panel.setSize(this.getPreferredSize());
            panel.setLayout(new GridBagLayout());
            panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

            c.gridx = 0;
            c.gridy = 0;
            c.weightx = 1;
            c.weighty = 0.05;
            c.fill = GridBagConstraints.HORIZONTAL;
            panel.add(topicLb, c);

            c.gridx = 1;
            c.gridy = 0;
            topicField.setColumns(10);
            panel.add(topicField, c);

            c.gridx = 0;
            c.gridy = 1;
            panel.add(msgLb, c);

            c.gridx = 1;
            c.gridy = 1;
            msgField.setColumns(10);
            panel.add(msgField, c);

            c.gridx = 0;
            c.gridy = 2;
            publishBtn.addActionListener((event) -> {
                if (topicField.getText().isEmpty()) {
                    return;
                }

                try {
                    ConnectPanel.this.getClient().pub(new ToolPublisher(topicField.getText(), msgField.getText()));
                } catch (Exception e) {
                    this.addLog(e.getMessage());
                }

            });
            panel.add(publishBtn, c);

            c.gridx = 1;
            c.gridy = 2;
            clearLogBtn.addActionListener((event) -> {
                this.logModel.clear();
            });
            panel.add(clearLogBtn, c);

            c.gridx = 0;
            c.gridy = 3;
            c.weighty = 1;
            c.gridwidth = 2;
            c.fill = GridBagConstraints.BOTH;

            logPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "Logs"));
            logs.setOpaque(false);
            logPanel.setOpaque(false);
            logPanel.getViewport().setOpaque(false);
            panel.add(logPanel, c);

            this.add(panel);

            this.pack();
            this.setVisible(false);
        }

        void addLog(String item) {
            logModel.addElement(item);
            int lastIndex = logs.getModel().getSize() - 1;
            if (lastIndex >= 0) {
                logs.ensureIndexIsVisible(lastIndex);
            }
        }

        class ToolPublisher extends Publisher {
            ToolPublisher(String topic, String msg) {
                super(ConnectPanel.this.getClient(), topic, msg);
            }

            @Override
            public void replyOk(Frame reply) {
                super.replyOk(reply);
                ToolFrame.this.addLog("OK \"" + this.getMsg() + "\" to \"" + this.getTopic() + "\": " + reply.getStrData());
            }

            @Override
            public void replyError(Frame reply) {
                super.replyError(reply);
                ToolFrame.this.addLog("ERR \"" + this.getMsg() + "\" to \"" + this.getTopic() + "\": " + reply.getStrData());
            }
        }
    }
}
