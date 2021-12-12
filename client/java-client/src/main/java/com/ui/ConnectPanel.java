package com.ui;

import com.core.client.BPSPClient;

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

    public ConnectPanel(AbstractUI mainUI) {
        this.mainUI = mainUI;
        this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, HEIGHT));
        this.setSize(this.getPreferredSize());
        this.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE),
                BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "Connector")
        ));
//        this.setBackground(Color.WHITE);

        this.add(inputForm);
        this.add(connectBtn);
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
}
