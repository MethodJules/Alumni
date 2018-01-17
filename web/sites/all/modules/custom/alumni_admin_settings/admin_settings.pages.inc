<?php

//Funktion zum Erzeugen der Form für Einstellungen zum Lebenslauf im Backend
function profile_cv_settings_form($form, &$form_state) {

    //Fieldset definieren
    $form['user_cv'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Lebenslauf Einstellungen'),
    );

    //Alle Felder in Drupal laden
    $fields = field_info_fields();

    $options = array();

    //options Array für Selectfeld mit den vorhandenen Feldern in Drupal erstellen
    foreach ($fields as $field) {
        $options[$field['field_name']] = $field['field_name'];
    }

    //Selectfeld für das Markup Feld PDF Export Button. Dieser Button wird beim Erstellen des PDFs ausgeblendet und daher muss das
    //Feld dazu bekannt sein
    $form['user_cv']['aup_cv_export_pdf_machine_name'] = array(
        '#type' => 'select',
        '#title' => t('Systemname des Markup Feldes für den PDF Export Button'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_export_pdf_machine_name', ''),
    );

    //Selectfeld für die Field Collection des Lebenslaufs. Wird fürs Exportieren des Lebenslaufs benötigt, damit die Exportfunktion weiß
    //wo nach dem Lebenslauf geschaut werden muss
    $form['user_cv']['aup_cv_machine_name'] = array(
        '#type' => 'select',
        '#title' => t('Systemname der Field Collection für den Lebenslauf'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_machine_name', ''),
    );

    //Liste mit Themes in Drupal laden
    $themes = list_themes();

    $options = array();

    //options Array für Selectfeld erstellen
    foreach ($themes as $theme) {
        $options[$theme->name] = $theme->name;
    }

    //Selectfeld um aktuelles Theme auszuwählen. Benötigt für Lebenslauf exportieren, damit der Pfad zur CSS mit der Formatierung des
    //Nutzerprofils bekannt ist.
    $form['user_cv']['aup_cv_theme_name'] = array(
        '#type' => 'select',
        '#title' => t('Aktuell eingeschaltetes Theme auswählen'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_theme_name', ''),
    );

    //Selectfeld um aktuelles Theme auszuwählen. Benötigt für Lebenslauf exportieren, damit der Pfad zur CSS mit der Formatierung des
    //Nutzerprofils bekannt ist.
    $form['user_cv']['aup_cv_theme_css_file'] = array(
        '#type' => 'textfield',
        '#title' => t('Pfad zur CSS Datei im Theme'),
        '#description' => t('Pfad innerhalb des Themes zur CSS Datei für die Formatierung des Lebenslaufs.' .
            'Wird beim Erzeugen des PDFs benötigt. Beispiel: /css/user-profile.css'),
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_theme_css_file', ''),
    );

    //Eingabefeld für den Abstand des Seitenrandes im PDF des Lebenslaufs
    $form['user_cv']['aup_cv_edge_margins'] = array(
        '#type' => 'textfield',
        '#title' => t('Abstand vom Inhalt zum Seitenrand im PDF festlegen'),
        '#description' => t('Abstand in cm angeben, ohne Leerzeichen und kein Komma. Beispiel: 1cm, 1.5cm, usw.'),
        '#default_value' => variable_get('aup_cv_edge_margins', '1cm'),
    );

    //Eingabefeld für die Größe des Bilds im PDF des Lebenslaufs
    $form['user_cv']['aup_cv_image_size'] = array(
        '#type' => 'textfield',
        '#title' => t('Größe des Bildes im PDF festlegen'),
        '#description' => t('Größe in cm angeben, ohne Leerzeichen und kein Komma. Beispiel: 1cm, 1.5cm, usw.'),
        '#default_value' => variable_get('aup_cv_image_size', '4cm'),
    );

    //Eingabefeld für den Namen einer Font. Dieser muss beim Konfigurieren eindeutig sein.
    $form['user_cv']['aup_cv_font_family'] = array(
        '#type' => 'textfield',
        '#title' => t('Name für Font Family festlegen'),
        '#description' => t('Name des Fonts angeben. Wird beim Erstellen des PDFs benötigt. ' .
            'Wenn eine Font hochgeladen wurde, muss dieser Wert gesetzt werden. ' .
            'Ein bereits verwendeter Name in Kombination mit hochgeladenen Fonts kann nicht noch einmal verwendet werden. ' .
            'In diesem Fall ist es notwendig einen neuen Namen zu vergeben.'),
        '#default_value' => variable_get('aup_cv_font_family', ''),
    );

    //Fieldset für Upload der Regular Font
    $form['user_cv']['aup_cv_regular_font'] = array(
        '#type' => 'fieldset',
        '#title' => t('Regular Font hochladen'),
    );

    //Wenn eine regular Font hochgeladen wurde, den Dateinamen anzeigen und einen Button zum löschen
    if (!empty(variable_get('aup_cv_regular_font_fid', '')))  {

        $file = file_load(variable_get('aup_cv_regular_font_fid', ''));

        if ($file) {
            $form['user_cv']['aup_cv_regular_font']['aup_regular_filename'] = array(
                '#markup' => '<span>' . $file->filename . '</span><br>',
            );

            $form['user_cv']['aup_cv_regular_font']['delete_regular_font_button'] = array(
                '#name' => 'delete_regular_font_button',
                '#type' => 'submit',
                '#value' => t('Datei löschen'),
            );

        }
    }

    //Uploadfeld für die Regular Font definieren
    $form['user_cv']['aup_cv_regular_font']['regular_font_upload'] = array(
        '#type' => 'file',
        '#description' => t('Erlaubte Erweiterungen: ttf. Für den Lebenslauf wird eine Regular Font und eine Bold Font benötigt.'),
    );


    //Fieldset für Upload der Bold Font
    $form['user_cv']['aup_cv_bold_font'] = array(
        '#type' => 'fieldset',
        '#title' => t('Bold Font hochladen'),
    );

    //Wenn eine bold Font hochgeladen wurde, den Dateinamen anzeigen und einen Button zum löschen
    if (!empty(variable_get('aup_cv_bold_font_fid', '')))  {

        $file = file_load(variable_get('aup_cv_bold_font_fid', ''));

        if ($file) {
            $form['user_cv']['aup_cv_bold_font']['aup_bold_filename'] = array(
                '#markup' => '<span>' . $file->filename . '</span><br>',
            );

            $form['user_cv']['aup_cv_bold_font']['delete_bold_font_button'] = array(
                '#name' => 'delete_bold_font_button',
                '#type' => 'submit',
                '#value' => t('Datei löschen'),
            );

        }
    }

    //Uploadfeld für die Bold Font definieren
    $form['user_cv']['aup_cv_bold_font']['bold_font_upload'] = array(
        '#type' => 'file',
        '#description' => t('Erlaubte Erweiterungen: ttf. Für den Lebenslauf wird eine Regular Font und eine Bold Font benötigt.'),
    );

    //Zusätzliche validate und submit Funktion definieren
    $form['#validate'][] = 'profile_cv_form_validate';
    $form['#submit'][] = 'profile_cv_form_submit';

    //system_settings_form erstellen, damit die Felder mit der Funktion variable_set automatisch gespeichert werden
    return system_settings_form($form);

}

//Validieren der Felder der profile_cv_form_submit Funktion
function profile_cv_form_validate($form, &$form_state) {

    //Datei aus Formular in Variable laden, falls eine angegeben wurde
    $file = file_save_upload('regular_font_upload', array(
        'file_validate_extensions' => array('ttf'),
    ));

    //Wenn die Variable gesetzt ist, die Datei an den richtigen Ort verschieben
    if ($file !== null) {
        if ($file) {
            if ($file = file_move($file, 'public://fonts')) {
                //Status auf permanent setzen, sonst wird es von Drupal nach 6 Stunden wieder gelöscht
                $file->status = FILE_STATUS_PERMANENT;

                //Datei speichern und die fid als Variable aufnehmen
                file_save($file);

                //Alte Fontdatei löschen, bevor die neu hochgeladene gesetzt wird
                $oldFile = file_load(variable_get('aup_cv_regular_font_fid'));
                file_delete($oldFile);

                variable_set('aup_cv_regular_font_fid', $file->fid);
            } else {
                form_set_error('file', t('Datei konnte nicht geschrieben werden.'));
            }
        } else {
            form_set_error('file', t('Keine Datei wurde hochgeladen.'));
        }
    }

    //Variable wieder auf null setzen, damit vom Anfangszustand ausgegangen werden kann
    $file = null;

    //Datei aus Formular in Variale laden, falls eine angegeben wurde
    $file = file_save_upload('bold_font_upload', array(
        'file_validate_extensions' => array('ttf'),
    ));

    //Wenn die Variable gesetzt ist, die Datei an den richtigen Ort verschieben
    if ($file !== null) {
        if ($file) {
            if ($file = file_move($file, 'public://fonts')) {
                //Status auf permanent setzen, sonst wird es von Drupal nach 6 Stunden wieder gelöscht
                $file->status = FILE_STATUS_PERMANENT;

                //Datei speichern und die fid als Variable aufnehmen
                file_save($file);

                //Alte Fontdatei löschen, bevor die neu hochgeladene gesetzt wird
                $oldFile = file_load(variable_get('aup_cv_bold_font_fid'));
                file_delete($oldFile);

                variable_set('aup_cv_bold_font_fid', $file->fid);
            } else {
                form_set_error('file', t('Datei konnte nicht geschrieben werden.'));
            }
        } else {
            form_set_error('file', t('Keine Datei wurde hochgeladen.'));
        }
    }
}

function profile_cv_form_submit($form, &$form_state) {

    //Wird ausgeführt, wenn der Button zum löschen der Regular Font betätigt wird
    if ($form_state['triggering_element']['#name'] === 'delete_regular_font_button') {
        //Datei laden
        $file = file_load(variable_get('aup_cv_regular_font_fid', ''));

        //Datei löschen
        file_delete($file);

        //Variable löschen
        variable_del('aup_cv_regular_font_fid');
    }

    //Wird ausgeführt, wenn der Button zum löschen der Bold Font betätigt wird
    if ($form_state['triggering_element']['#name'] === 'delete_bold_font_button') {
        //Datei laden
        $file = file_load(variable_get('aup_cv_bold_font_fid', ''));

        //Datei löschen
        file_delete($file);

        //Variable löschen
        variable_del('aup_cv_bold_font_fid');
    }
}

//Funktion zum Erzeugen der Form für Einstellungen für Projekte im Backend
function projects_settings_form($form, &$form_state) {
    $options = array();

    //Vorhandene Content Types laden
    $types = node_type_get_types();

    //options Array für Selectfeld aufbauen
    foreach ($types as $type) {
        $options[$type->type] = $type->type;
    }

    //Fieldset für Projekte Einstellungen
    $form['projects'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Projekte Einstellungen'),
    );

    //Selectfeld zur Auswahl des Content Types erstellen. Wird für die Mitgliederverwaltung der Projekte benötigt. Modul alumni_projects muss den Content Type kennen.
    $form['projects']['aup_projects_content_type'] = array(
        '#type' => 'select',
        '#title' => t('Systemname des Content Types für Projekte'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_projects_content_type', ''),
    );

    //system_settings_form erstellen, damit die Felder mit der Funktion variable_set automatisch gespeichert werden
    return system_settings_form($form);
}

function interests_settings_form($form, &$form_state) {

    $options = array();

    $vocabs = taxonomy_get_vocabularies();

    foreach ($vocabs as $data) {
        $options[$data->machine_name] = $data->machine_name;
    }

    $form['interests'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Projekte Einstellungen'),
    );

    $form['interests']['aup_interests_taxonomy'] = array(
        '#type' => 'select',
        '#title' => t('Systemname der Taxonomy Liste für Interessen'),
        '#description' => t('Wird für das Verwalten der Vorschläge zu Interessen im Backend benötigt.'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_interests_taxonomy', ''),
    );

    //Alle Felder in Drupal laden
    $fields = field_info_fields();

    $options = array();

    //options Array für Selectfeld mit den vorhandenen Feldern in Drupal erstellen
    foreach ($fields as $field) {
        $options[$field['field_name']] = $field['field_name'];
    }

    $form['interests']['aud_interests_user'] = array(
        '#type' => 'select',
        '#title' => t('Systemname des Feldes Interessen beim Benutzerprofil'),
        '#description' => t('Wird für die automatische Übereinstimmung zwischen Interessen des Benutzers und Stellenangeboten etc. benötigt.'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aud_interests_user', ''),
    );

    $form['interests']['aud_interests_job_offers'] = array(
        '#type' => 'select',
        '#title' => t('Systemname des Feldes Interessen beim Content Type für Stellenangebote'),
        '#description' => t('Wird für die automatische Übereinstimmung zwischen Interessen des Benutzers und Stellenangeboten etc. benötigt.'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aud_interests_job_offers', ''),
    );

    //system_settings_form erstellen, damit die Felder mit der Funktion variable_set automatisch gespeichert werden
    return system_settings_form($form);
}