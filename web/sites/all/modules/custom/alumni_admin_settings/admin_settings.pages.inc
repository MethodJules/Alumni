<?php

function profile_cv_settings_form($form, &$form_state) {

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
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_export_pdf_machine_name', ''),
    );

    $form['user_cv']['aup_cv_machine_name'] = array(
        '#type' => 'select',
        '#title' => t('Systemname der Field Collection für den Lebenslauf'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_machine_name', ''),
    );

    $themes = list_themes();

    $options = array();

    foreach ($themes as $theme) {
        $options[$theme->name] = $theme->name;
    }

    $form['user_cv']['aup_cv_theme_name'] = array(
        '#type' => 'select',
        '#title' => t('Aktuell eingeschaltetes Theme auswählen'),
        '#options' => $options,
        '#empty_option' => SELECT_DEFAULT,
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_theme_name', ''),
    );

    $form['user_cv']['aup_cv_theme_css_file'] = array(
        '#type' => 'textfield',
        '#title' => t('Pfad zur CSS Datei im Theme'),
        '#description' => t('Pfad innerhalb des Themes zur CSS Datei für die Formatierung des Lebenslaufs.' .
            'Wird beim Erzeugen des PDFs benötigt. Beispiel: /css/user-profile.css'),
        '#required' => TRUE,
        '#default_value' => variable_get('aup_cv_theme_css_file', ''),
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

    $form['user_cv']['aup_cv_font_family'] = array(
        '#type' => 'textfield',
        '#title' => t('Name für Font Family festlegen'),
        '#description' => t('Name des Fonts angeben. Wird beim Erstellen des PDFs benötigt. ' .
            'Wenn eine Font hochgeladen wurde, muss dieser Wert gesetzt werden. ' .
            'Ein bereits verwendeter Name in Kombination mit hochgeladenen Fonts kann nicht noch einmal verwendet werden. ' .
            'In diesem Fall ist es notwendig einen neuen Namen zu vergeben.'),
        '#default_value' => variable_get('aup_cv_font_family', ''),
    );

    $form['user_cv']['aup_cv_regular_font'] = array(
        '#type' => 'fieldset',
        '#title' => t('Regular Font hochladen'),
    );

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

    $form['user_cv']['aup_cv_regular_font']['regular_font_upload'] = array(
        '#type' => 'file',
        '#description' => t('Erlaubte Erweiterungen: ttf. Für den Lebenslauf wird eine Regular Font und eine Bold Font benötigt.'),
    );


    $form['user_cv']['aup_cv_bold_font'] = array(
        '#type' => 'fieldset',
        '#title' => t('Bold Font hochladen'),
    );

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

    $form['user_cv']['aup_cv_bold_font']['bold_font_upload'] = array(
        '#type' => 'file',
        '#description' => t('Erlaubte Erweiterungen: ttf. Für den Lebenslauf wird eine Regular Font und eine Bold Font benötigt.'),
    );

    $form['#validate'][] = 'profile_cv_form_validate';
    $form['#submit'][] = 'profile_cv_form_submit';

    return system_settings_form($form);

}

function profile_cv_form_validate($form, &$form_state) {


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
    /*if (isset($form_state['values']['aup_cv_regular_font'])) {

        $file = file_load($form_state['values']['aup_cv_regular_font']);
        if ($file) {
            $file->status = FILE_STATUS_PERMANENT;
            file_save($file);
            variable_set('aup_cv_regular_font_fid', $file->fid);
        }
    }
    else {
        $file = file_load(variable_get('aup_cv_regular_font_fid', ''));
        if ($file->fid) {
            file_delete($file, TRUE);
        }
    }*/
}

function profile_cv_form_submit($form, &$form_state) {

    if ($form_state['triggering_element']['#name'] === 'delete_regular_font_button') {
        //Datei laden
        $file = file_load(variable_get('aup_cv_regular_font_fid', ''));

        //Datei löschen
        file_delete($file);

        //Variable löschen
        variable_del('aup_cv_regular_font_fid');
    }

    if ($form_state['triggering_element']['#name'] === 'delete_bold_font_button') {
        //Datei laden
        $file = file_load(variable_get('aup_cv_bold_font_fid', ''));

        //Datei löschen
        file_delete($file);

        //Variable löschen
        variable_del('aup_cv_bold_font_fid');
    }
}