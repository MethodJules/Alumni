<?php

function add_event($form, &$form_state) {
    module_load_include('inc', 'node', 'node.pages');
    $node_form = new stdClass;
    $node_form->type = 'veranstaltungen';
    $node_form->language = LANGUAGE_NONE;
    $form = drupal_get_form('veranstaltungen_node_form', $node_form);
    return $form;
}