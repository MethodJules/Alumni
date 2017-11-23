<?php

function profile_cv_form($form, &$form_state) {

    $form['user_cv'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Lebenslauf Einstellungen'),
    );

    $fields = field_info_fields();

    $options = array();

    foreach ($fields as $field) {
        $options[$field['field_name']] = $field['field_name'];
    }

    $form['user_cv']['aup_cv_export_pdf_machine_name'] = array(
        '#type' => 'select',
        '#title' => t('Systemname des Markup Feldes für den PDF Export Button'),
        '#options' => $options,
        '#empty_option' => t('- Keine -'),
        '#default_value' => variable_get('aup_cv_export_pdf_machine_name', ''),
    );

    $form['user_cv']['aup_cv_machine_name'] = array(
        '#type' => 'select',
        '#title' => t('Systemname der Field Collection für den Lebenslauf'),
        '#options' => $options,
        '#empty_option' => t('- Keine -'),
        '#default_value' => variable_get('aup_cv_machine_name', ''),
    );

    $form['user_cv']['aup_cv_edge_margins'] = array(
        '#type' => 'textfield',
        '#title' => t('Abstand vom Inhalt zum Seitenrand im PDF festlegen'),
        '#description' => t('Abstand in cm angeben, ohne Leerzeichen und kein Komma. Beispiel: 1cm, 1.5cm, usw.'),
        '#default_value' => variable_get('aup_cv_edge_margins', '1cm'),
    );

    $form['user_cv']['aup_cv_image_size'] = array(
        '#type' => 'textfield',
        '#title' => t('Größe des Bildes im PDF festlegen'),
        '#description' => t('Größe in cm angeben, ohne Leerzeichen und kein Komma. Beispiel: 1cm, 1.5cm, usw.'),
        '#default_value' => variable_get('aup_cv_image_size', '4cm'),
    );

    $form = system_settings_form($form);

    return $form;
}