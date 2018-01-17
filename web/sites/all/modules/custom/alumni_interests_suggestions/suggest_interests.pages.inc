<?php

function suggest_interest_form($form, &$form_state) {
    $form = array();

    $form['interest_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#default_value' => '',
    );

    $form['suggest_interest_input_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
    );
    return $form;
}

function suggest_interest_form_validate($form, $form_state) {
    if (empty($form_state['values']['interest_input'])) {
        form_set_error('interest_input', t('Leerer Vorschlag kann nicht abgeschickt werden.'));
    }
}

function suggest_interest_form_submit($form, &$form_state) {

    $query = db_select('alumni_interests_suggestions', 'ais');
    $query->fields('ais', array('id'));
    $query->orderBy('id', 'DESC');
    $result = $query->execute();

    $result = $result->fetchAssoc();

    $id = 0;

    if ($result) {
        dpm($result);
        $id = $result['id'];
        $id++;
    }

    db_insert('alumni_interests_suggestions')
        ->fields(array(
            'id' => $id,
            'name' => $form_state['values']['interest_input'],
        ))
        ->execute();
    drupal_set_message(t('Vorschlag wurde abgeschickt.'));
}