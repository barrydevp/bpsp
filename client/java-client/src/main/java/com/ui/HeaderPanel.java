package com.ui;

import javax.swing.*;
import java.awt.*;

public class HeaderPanel extends JPanel {
    static final int BORDER_SIZE = 5;
    static final int HEIGHT = 60;

    JLabel titleLabel = new JLabel();

    public HeaderPanel() {
        this("BPSP UI");
    }

    public HeaderPanel(String title) {
        titleLabel.setFont(new Font("Helvetica", Font.BOLD, 36));
        titleLabel.setForeground(Color.RED);
        titleLabel.setText(title);

        this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, HEIGHT));
        this.setSize(this.getPreferredSize());
        this.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE),
                BorderFactory.createLineBorder(Color.PINK)
        ));
        this.add(titleLabel, JPanel.CENTER_ALIGNMENT);
    }
}
