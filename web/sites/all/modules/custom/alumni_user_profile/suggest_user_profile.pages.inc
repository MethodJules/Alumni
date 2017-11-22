<?php

function get_suggestions_form($form, $form_state) {
    //CSS laden
    drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_media.css', array('group' => CSS_DEFAULT));

    $form = array();

    //Abschnitt Schulen vorschlagen
    $result = db_select('alumni_suggestions', 'ex')
        ->fields('ex', array('id', 'name'))
        ->condition('category', 'schools', '=')
        ->execute();

    $form['school_fieldset'] = array(
        '#type' => 'fieldset',
        '#title' => t('Vorgeschlagene Schulen'),
        '#tree' => TRUE,
    );

    $form['school_fieldset']['table_start'] = array(
        '#markup' => '<table><tr><th width="30px"></th><th>' . t('Vorschlag') . '</th><th width="110px">' . t('Löschen') . '</th></tr>'
    );

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('aup_school_machine_name'));
    $vid = $vocab->vid;

    $taxonomy = taxonomy_get_tree($vid);

    $options = array();
    $options[SELECT_DEFAULT] = SELECT_DEFAULT;

    foreach ($taxonomy as $data) {

        if ($data->parents[0] === '0') {
            $options[$data->tid] = $data->name;
        }
    }

    $counter = 0;
    while ($data = $result->fetchAssoc()) {
        $form['school_fieldset']['schools'][$counter]['row_start'] = array(
            '#markup' => '<tr>'
        );

        $form['school_fieldset']['schools'][$counter]['school_check'] = array(
            '#type' => 'checkbox',
            '#prefix' => '<td>',
            '#suffix' => '</td>'
        );

        $form['school_fieldset']['schools'][$counter]['school_select'] = array(
            '#type' => 'select',
            '#options' => $options,
            '#default_value' => SELECT_DEFAULT,
            '#prefix' => '<td><div class="left-floated-element">',
            '#suffix' => '</div>',
        );

        $form['school_fieldset']['schools'][$counter]['school_input'] = array(
            '#type' => 'textfield',
            '#default_value' => $data['name'],
            '#prefix' => '<div class="left-floated-element">',
            '#suffix' => '</div><div class="clear-float"></div></td>',
        );

        $form['school_fieldset']['schools'][$counter]['school_delete'] = array(
            '#type' => 'submit',
            '#name' => 'del_suggest_school' . $data['id'],
            '#value' => t('Löschen'),
            '#submit' => array('delete_suggest_school_submit'),
            '#attributes' => array('suggest-id' => $data['id']),
            '#prefix' => '<td>',
            '#suffix' => '</td>',
        );

        $form['school_fieldset']['schools'][$counter]['suggest_id'] = array(
            '#type' => 'hidden',
            '#value' => $data['id'],
        );

        $form['school_fieldset']['schools'][$counter]['row_end'] = array(
            '#markup' => '</tr>'
        );

        $counter++;
    }

    $form['school_fieldset']['table_end'] = array(
        '#markup' => '</table>'
    );

    $form['school_save_button'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
        '#submit' => array('save_suggest_school_submit'),
        '#validate' => array('save_suggest_school_validate'),

    );

    //Abschnitt Interessen vorschlagen

    $result = db_select('alumni_suggestions', 'ex')
        ->fields('ex', array('id', 'name'))
        ->condition('category', 'interests', '=')
        ->execute();

    $form['interest_fieldset'] = array(
        '#type' => 'fieldset',
        '#title' => t('Vorgeschlagene Interessen'),
        '#tree' => TRUE,
    );

    $form['interest_fieldset']['table_start'] = array(
        '#markup' => '<table><tr><th width="30px"></th><th>' . t('Vorschlag') . '</th><th width="110px">' . t('Löschen') . '</th></tr>'
    );

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('aup_interest_machine_name'));
    $vid = $vocab->vid;

    $taxonomy = taxonomy_get_tree($vid);

    $options = array();
    $options[SELECT_DEFAULT] = SELECT_DEFAULT;

    foreach ($taxonomy as $data) {

        if ($data->parents[0] === '0') {
            $options[$data->tid] = $data->name;
        }
    }

    $counter = 0;
    while ($data = $result->fetchAssoc()) {
        $form['interest_fieldset']['interest'][$counter]['row_start'] = array(
            '#markup' => '<tr>'
        );

        $form['interest_fieldset']['interest'][$counter]['interest_check'] = array(
            '#type' => 'checkbox',
            '#prefix' => '<td>',
            '#suffix' => '</td>'
        );

        $form['interest_fieldset']['interest'][$counter]['interest_select'] = array(
            '#type' => 'select',
            '#options' => $options,
            '#default_value' => SELECT_DEFAULT,
            '#prefix' => '<td><div class="left-floated-element">',
            '#suffix' => '</div>',
        );

        $form['interest_fieldset']['interest'][$counter]['interest_input'] = array(
            '#type' => 'textfield',
            '#default_value' => $data['name'],
            '#prefix' => '<div class="left-floated-element">',
            '#suffix' => '</div><div class="clear-float"></div></td>',
        );

        $form['interest_fieldset']['interest'][$counter]['interest_delete'] = array(
            '#type' => 'submit',
            '#name' => 'del_suggest_interest' . $data['id'],
            '#value' => t('Löschen'),
            '#submit' => array('delete_suggest_interest_submit'),
            '#attributes' => array('suggest-id' => $data['id']),
            '#prefix' => '<td>',
            '#suffix' => '</td>',
        );

        $form['interest_fieldset']['interest'][$counter]['suggest_id'] = array(
            '#type' => 'hidden',
            '#value' => $data['id'],
        );

        $form['interest_fieldset']['interest'][$counter]['row_end'] = array(
            '#markup' => '</tr>'
        );

        $counter++;
    }

    $form['interest_fieldset']['table_end'] = array(
        '#markup' => '</table>'
    );

    $form['interest_save_button'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
        '#submit' => array('save_suggest_interest_submit'),
        '#validate' => array('save_suggest_interest_validate'),
    );

    return $form;
}

function save_suggest_school_validate($form, &$form_state) {


    $index = 0;
    foreach ($form_state['values']['school_fieldset']['schools'] as $value) {
        if ($value['school_check'] == 1 && $value['school_select'] == SELECT_DEFAULT) {
            form_set_error('school_fieldset][schools][' . $index . '][school_select', t('Bei selektierten Vorschlägen eine Obergruppe auswählen.'));
        }

        $index++;
    }
}

function save_suggest_school_submit($form, &$form_state) {

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('aup_school_machine_name'));
    $vid = $vocab->vid;

    foreach ($form_state['values']['school_fieldset']['schools'] as $value) {
        if ($value['school_check'] == 1) {
            $term = new stdClass();
            $term->name = $value['school_input'];
            $term->vid = $vid;
            $term->parent = $value['school_select'];
            taxonomy_term_save($term);


            db_delete('alumni_suggestions')
                ->condition('id', $value['suggest_id'], '=')
                ->execute();
        }
    }

    drupal_set_message(t('Gespeichert'));
}

function save_suggest_interest_validate($form, &$form_state) {


    $index = 0;
    foreach ($form_state['values']['interest_fieldset']['interest'] as $value) {
        if ($value['interest_check'] == 1 && $value['interest_select'] == SELECT_DEFAULT) {
            form_set_error('interest_fieldset][interest][' . $index . '][interest_select', t('Bei selektierten Vorschlägen eine Obergruppe auswählen.'));
        }

        $index++;
    }
}

function save_suggest_interest_submit($form, &$form_state) {

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('aup_interest_machine_name'));
    $vid = $vocab->vid;

    foreach ($form_state['values']['interest_fieldset']['interest'] as $value) {
        if ($value['interest_check'] == 1) {
            $term = new stdClass();
            $term->name = $value['interest_input'];
            $term->vid = $vid;
            $term->parent = $value['interest_select'];
            taxonomy_term_save($term);


            db_delete('alumni_suggestions')
                ->condition('id', $value['suggest_id'], '=')
                ->execute();
        }
    }

    drupal_set_message(t('Gespeichert'));
}

function delete_suggest_school_submit($form, &$form_state) {
    $id = $form_state['triggering_element']['#attributes']['suggest-id'];

    db_delete('alumni_suggestions')
        ->condition('id', $id, '=')
        ->execute();

    drupal_set_message(t('Vorschlag wurde gelöscht.'));
}

function delete_suggest_interest_submit($form, &$form_state) {
    $id = $form_state['triggering_element']['#attributes']['suggest-id'];

    db_delete('alumni_suggestions')
        ->condition('id', $id, '=')
        ->execute();

    drupal_set_message(t('Vorschlag wurde gelöscht.'));
}

function suggest_school_form($form, &$form_state) {

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_media.css', array('group' => CSS_DEFAULT));

    $form = array();

    $form['school_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Folgende Schule schlage ich vor'),
        '#default_value' => '',
    );

    $form['suggest_school_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Vorschlag abschicken'),
    );
    return $form;
}

function suggest_school_form_validate($form, $form_state) {
    if (empty($form_state['values']['school_input'])) {
        form_set_error('school_input', t('Leerer Vorschlag kann nicht abgeschickt werden.'));
    }
}

function suggest_school_form_submit($form, &$form_state) {

    $result = db_select('alumni_suggestions', 'ex')
        ->fields('ex', array('id'))
        ->orderBy('id', 'DESC')
        ->execute()
        ->fetchAssoc();

    $id = 0;

    if ($result) {
        $id = $result['id'];
        $id++;
    }

    db_insert('alumni_suggestions')
        ->fields(array(
            'id' => $id,
            'category' => 'schools',
            'name' => $form_state['values']['school_input'],
        ))
        ->execute();
    drupal_set_message(t('Vorschlag wurde abgeschickt.'));
}

function suggest_interest_form($form, &$form_state) {

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'alumni_user_profile') . '/css/aup_media.css', array('group' => CSS_DEFAULT));

    $form = array();

    $form['interest_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Folgende Interesse schlage ich vor'),
        '#default_value' => '',
    );

    $form['suggest_interest_input_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Vorschlag abschicken'),
    );
    return $form;
}

function suggest_interest_form_validate($form, $form_state) {
    if (empty($form_state['values']['interest_input'])) {
        form_set_error('interest_input', t('Leerer Vorschlag kann nicht abgeschickt werden.'));
    }
}

function suggest_interest_form_submit($form, &$form_state) {

    $result = db_select('alumni_suggestions', 'ex')
        ->fields('ex', array('id'))
        ->orderBy('id', 'DESC')
        ->execute()
        ->fetchAssoc();

    $id = 0;

    if ($result) {
        $id = $result['id'];
        $id++;
    }

    db_insert('alumni_suggestions')
        ->fields(array(
            'id' => $id,
            'category' => 'interests',
            'name' => $form_state['values']['interest_input'],
        ))
        ->execute();
    drupal_set_message(t('Vorschlag wurde abgeschickt.'));
}