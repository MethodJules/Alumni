<?php

function set_ids_form($form, &$form_state) {
    $form = array();


    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    $form = array();

    $form['machine_names'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Systemnamen festlegen'),
    );

    $form['machine_names']['eup_school_machine_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Systemname der Taxonomie für Schulen'),
        '#default_value' => variable_get('eup_school_machine_name', ''),
    );

    $form['machine_names']['eup_jfproject_machine_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Systemname der Taxonomie für Jugend Forscht Projekte'),
        '#default_value' => variable_get('eup_jfproject_machine_name', ''),
    );

    $form['machine_names']['eup_interest_machine_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Systemname der Taxonomie für Interessen'),
        '#default_value' => variable_get('eup_interest_machine_name', ''),
    );

    $form['interests'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Interessen'),
    );

    $form['interests']['eup_interest_info_text'] = array(
        '#title' => t('Info Nachricht für Interessen im Benutzerprofil'),
        '#type' => 'text_format',
        '#resizable' => false,
        '#default_value' => variable_get('eup_interest_info_text', '')['value'],
        '#format' => 'full_html',
    );

    $form = system_settings_form($form);

    return $form;
}