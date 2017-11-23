<?php

function profile_cv_form($form, &$form_state) {
    //CSS laden
    //drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_custom.css', array('group' => CSS_DEFAULT));
    //drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_media.css', array('group' => CSS_DEFAULT));


    $form = array();

    $form['machine_names'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Systemnamen festlegen'),
    );

    $form['machine_names']['aup_jfproject_machine_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Systemname der Taxonomie für Jugend Forscht Projekte'),
        '#default_value' => variable_get('aup_jfproject_machine_name', ''),
    );

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

    $form = system_settings_form($form);

    return $form;
}