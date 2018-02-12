<?php

function privacy_settings_form($form, &$form_state)
{
    $options = array();

    $form['privacy'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Datenschutz/Privatsphäre Einstellungen'),
    );

    $query = db_select('user_relationship_types', 'ur');
    $query->fields('ur', array('rtid', 'name'));
    $result = $query->execute();

    foreach ($result as $data) {
        $options[$data->rtid] = $data->name;
    }

    $form['privacy']['aup_friend_type'] = array(
        '#type' => 'select',
        '#title' => t('Auswahl des Beziehungen-Typs Freunde'),
        '#description' => t('Auswahl des Typs Freunde aus der Liste der Beziehungen des Moduls User Relationships.
            Wird dafür benötigt, damit die Freundschaft überprüft werden kann und ob der Benutzer die Informationen einsehen darf, die als "dürfen nur Freunde sehen" gekennzeichnet sind.'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_friend_type', ''),
    );

    $form['privacy']['aup_prefix_privacy_fields'] = array(
        '#type' => 'textfield',
        '#title' => t('Prefix für Felder'),
        '#description' => t('Den Prefix der Felder angeben, bei denen in den Einstellungen des Feldes eine Checkbox erscheinen soll, ob der Benutzer 
            die Privatsphäreeinstellungen für dieses Feld verwenden darf. Anhand dieser Einstellung wird die Auswahlliste für Öffentlich, Freunde und Nur ich den
            einzelnen Feldern zugeordnet. Der Systemname für Felder muss mit dem ausgewähltem Präfix anfangen, z.B. "field_user"'),
        '#required' => TRUE,
        '#default_value' => variable_get('aup_prefix_privacy_fields', ''),
    );

    $form['privacy']['other_settings'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Datenschutz weiter Einstellungen'),
    );

    //Alle Felder in Drupal laden
    $fields = field_info_fields();

    $options = array();

    //options Array für Selectfeld mit den vorhandenen Feldern in Drupal erstellen
    foreach ($fields as $field) {
        $options[$field['field_name']] = $field['field_name'];
    }

    $form['privacy']['other_settings']['aup_other_settings_field'] = array(
        '#type' => 'select',
        '#title' => t('Feld auswählen'),
        '#description' => t('Feld auswählen, bei dem entweder davor oder danach die weiteren Einstellungen zum Datenschutz angezeigt werden sollen.
            Dazu gehört unteranderem, ob der Benutzer anderen Benutzern als Freund vorgeschlagen werden soll.'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_other_settings_field', ''),
    );

    $form['privacy']['other_settings']['aup_other_settings_position'] = array(
        '#type' => 'radios',
        '#title' => t('Position auswählen'),
        '#description' => t('Auswählen, ob die weiteren Einstellungen vor oder nach dem Feld angezeigt werden sollen.'),
        '#options' => array(
            'before' => t('davor'),
            'after' => t('danach'),
            'none' => t('gar nicht'),
        ),
        '#default_value' => variable_get('aup_other_settings_position', ''),
     );

    //system_settings_form erstellen, damit die Felder mit der Funktion variable_set automatisch gespeichert werden
    return system_settings_form($form);
}