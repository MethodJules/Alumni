<?php

//Funktion zum Aufbauen der Form
function extended_user_profile_cv_form($form, &$form_state, $user_id) {

    //Überprüfen ob Benutzer mit der ID existiert
    $userLoad = user_load($user_id);
    if (!$userLoad) {
        drupal_not_found();
        drupal_exit();
    }

    //Aktuellen angemeldeten Benutzer holen
    global $user;

    //Überprüfen, ob der Benutzer selber seine Seite aufruft oder ein Administrator dieses tut. Andere Benutzer sollen die Seite nicht aufrufen dürfen
    if ($user_id != $user->uid && !user_has_role(3, user_load($user->uid))) {
        drupal_access_denied();
        drupal_exit();
    }

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    //Felder für Dantebankabfrage
    $fields = array(
        'image_id',
        'salutation',
        'title',
        'first_name',
        'last_name',
        'birthday_day',
        'birthday_month',
        'birthday_year',
        'email',
        'street',
        'city',
        'zip_code',
        'telephone',
        'nationality',
        'family',
        'practical_experience',
        'education',
        'language_skills',
        'special_knowledge'
    );

    //Wenn die ID der aufgerufenen Seite ungleich der ID des aktuell angemeldeten Benutzer ist und der Benutzer den Administratoren angehört, dann bleibt user_id,
    //ansonsten wird user_id die ID des aktuell angemeldeten Benutzers.
    $user_id = ($user_id != $user->uid && user_has_role(3, user_load($user->uid))) ? $user_id : $user->uid;
    $result = db_select('extended_user_profile_cv', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    //Feld für versteckte User ID ins Array laden
    $form['cv_user_id'] = array(
        '#type' => 'hidden',
        '#value' => $user_id,
    );

    //Fieldset für Persönliches in Array laden
    $form['personal'] = array(
        '#type' => 'fieldset',
        '#title' => t('Persönliches'),
        '#tree' => FALSE,
    );

    //Wenn ein Bild existiert, dann die Informationen aus dem Bild laden, HTML für Bild und Löschbutton erzeugen
    if ($result['image_id'] != 0) {

        $image = file_load($result['image_id']);

        $form['personal']['cv_image'] = array(
            '#markup' => '<img src="' . file_create_url($image->uri) . '" width="200px"><br>'
        );

        $form['personal']['delete_image'] = array(
            '#type' => 'submit',
            '#value' => t('Bild löschen'),
            '#submit' => array('delete_image_button_submit'),
        );

        //Wird benötigt, wenn das Bild über den Löschbutton entfernt werden sol
        $form['personal']['cv_image_id'] = array(
            '#type' => 'hidden',
            '#value' => $result['image_id'],
        );
    }

    //Uploadfeld für Bild ins Array laden
    $form['personal']['cv_image_upload'] = array(
        '#type' => 'file',
        '#title' => t('Bild hochladen'),
        '#description' => t('Erlaubte Erweiterungen: jpg, jpeg, png, gif'),
    );

    //Selectfeld für Anrede ins Array laden
    $form['personal']['salutation'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Anrede'),
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Herr') => t('Herr'),
            t('Frau') => t('Frau'),
        ),
        '#default_value' => (isset($result['salutation']) && !empty($result['salutation'])) ? $result['salutation'] : '-',
    );

    //Eingabfeld für Titel ins Array laden
    $form['personal']['title'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Akad. Titel'),
        '#size' => 10,
        '#default_value' => (isset($result['title']) && !empty($result['title'])) ? $result['title'] : '',
    );

    //Eingabefeld für Vorname ins Array laden
    $form['personal']['firstname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Vorname'),
        '#default_value' => (isset($result['first_name']) && !empty($result['first_name'])) ? $result['first_name'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    //Eingabefeld für Nachname ins Array laden
    $form['personal']['lastname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Nachname'),
        '#default_value' => (isset($result['last_name']) && !empty($result['last_name'])) ? $result['last_name'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>'
    );

    //Array mit Tagen erstellen
    $days = array(SELECT_DEFAULT => SELECT_DEFAULT);
    for ($i = 1; $i <= 31; $i++) {
        $days[strval($i)] = strval($i);
    }

    //Array mit Monaten erstellen
    $months = array(
        SELECT_DEFAULT => SELECT_DEFAULT,
        t('Januar') => t('Januar'),
        t('Februar') => t('Februar'),
        t('März') => t('März'),
        t('April') => t('April'),
        t('Mai') => t('Mai'),
        t('Juni') => t('Juni'),
        t('Juli') => t('Juli'),
        t('August') => t('August'),
        t('September') => t('September'),
        t('Oktober') => t('Oktober'),
        t('November') => t('November'),
        t('Dezember') => t('Dezember'),
    );

    //Array mit Jahren erstellen
    $years = array(SELECT_DEFAULT => SELECT_DEFAULT);

    for ($i = date("Y"); $i >= 1950; $i--) {
        $years[strval($i)] = strval($i);
    }

    //Felder für Geburtstag ins Array laden
    $form['personal']['birthday'] = array(
        '#markup' => '<b>' . t('Geburtstag') . ':</b><br>',
    );

    $form['personal']['birthday_day'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Tag'),
        '#options' => $days,
        '#default_value' => (isset($result['birthday_day']) && !empty($result['birthday_day'])) ? $result['birthday_day'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['personal']['birthday_month'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Monat'),
        '#options' => $months,
        '#default_value' => (isset($result['birthday_month']) && !empty($result['birthday_month'])) ? $result['birthday_month'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['personal']['birthday_year'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Jahr'),
        '#options' => $years,
        '#default_value' => (isset($result['birthday_year']) && !empty($result['birthday_year'])) ? $result['birthday_year'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>'
    );

    //Eingabefeld für E-Mail ins Array laden
    $form['personal']['email'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('E-Mail'),
        '#default_value' => (isset($result['email']) && !empty($result['email'])) ? $result['email'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    //Eingabefeld für Telefon ins Array laden
    $form['personal']['telephone'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Telefon/Mobil'),
        '#default_value' => (isset($result['telephone']) && !empty($result['telephone'])) ? $result['telephone'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>'
    );

    //Eingabefeld für Straße ins Array laden
    $form['personal']['street'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Straße'),
        '#default_value' => (isset($result['street']) && !empty($result['street'])) ? $result['street'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    //Eingabefeld für Stadt ins Array laden
    $form['personal']['city'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Wohnort'),
        '#default_value' => (isset($result['city']) && !empty($result['city'])) ? $result['city'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    //Eingabefeld für PLZ ins Array laden
    $form['personal']['zip_code'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('PLZ'),
        '#default_value' => (isset($result['zip_code']) && $result['zip_code'] != 0) ? $result['zip_code'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>'
    );

    //Eingabefeld für Nationalität ins Array laden
    $form['personal']['nationality'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Staatsangehörigkeit'),
        '#default_value' => (isset($result['nationality']) && !empty($result['nationality'])) ? $result['nationality'] : '',
    );

    //Selectfeld für Familienstand ins Array laden
    $form['personal']['family'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Familienstand'),
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('ledig') => t('ledig'),
            t('verheiratet') => t('verheiratet'),
            t('geschieden') => t('geschieden'),
            t('verwitwet') => t('verwitwet'),
            t('Ehe aufgehoben') => t('Ehe aufgehoben'),
            t('eingetragene Lebenspartnerschaft') => t('eingetragene Lebenspartnerschaft'),
            t('aufgelöste Lebenspartnerschaft') => t('aufgelöste Lebenspartnerschaft'),
            t('durch Tod aufgelöste Lebenspartnerschaft') => t('durch Tod aufgelöste Lebenspartnerschaft'),
        ),
        '#default_value' => (isset($result['family']) && !empty($result['family'])) ? $result['family'] : SELECT_DEFAULT,
    );

    //Fieldset für praktische Erfahrunfen ins Array laden
    $form['practical_experience'] = array(
        '#type' => 'fieldset',
        '#title' => t('Praxiserfahrungen'),
        '#prefix' => '<div id="replace-practical-experience">', //Der Bereich wird beim Nachladen von dynmaischen Feldern ersetzt
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    //Überprüfen ob ein Button geklickt wurde. Passiert beim dynamischen Nachladen von Feldern
    if (!array_key_exists('clicked_button', $form_state)) {

        //Beim Aufruf der Seite überprüfen, ob bereits Daten in der Datenbank stehen
        if (isset($result['practical_experience'])) {

            //String in Array umwandeln, überprüfen, ob das Array Inhalt hat und anschließend das Array iterieren und
            //die Funktion create_practical_experience mit den Daten aus der Datenbank aufrufen. Felder werden mit
            //Daten aus der Datenbank befüllt
            $practicalExperience = unserialize($result['practical_experience']);
            if (count($practicalExperience) != 0) {
                $index = 0;

                foreach ($practicalExperience as $key => $value) {
                    create_practical_experience($form, $index, $value);
                    $index++;
                }

            } else {
                //Andernfalls leere Felder erzeugen
                create_practical_experience($form, 0, null);
            }

        } else {
            //Andernfalls leere Felder erzeugen
            create_practical_experience($form, 0, null);
        }
    } else {


        // Beim dynamischen Nachladen von Feldern alle bisher existierenden iterieren und die Felder aufbauen
        foreach ($form_state['values']['practical_experience'] as $i => $value) {
            create_practical_experience($form, $i, null);
        }

        //Wenn der Button für praktische Erfahrung geklickt wurde, einen zusätzlichen Bereich mit Feldern laden
        if ($form_state['clicked_button']['#name'] == 'practical_experience_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['practical_experience']);
            create_practical_experience($form, $last, null);
        }
    }

    //Mehr Button für praktische Erfahrung ins Array laden
    $form['practical_experience']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'practical_experience_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_practical_experience',
            'wrapper' => 'replace-practical-experience',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    //Bereich für Ausbildung ist analog zu den praktischen Erfahrungen
    $form['education'] = array(
        '#type' => 'fieldset',
        '#title' => t('Ausbildung'),
        '#prefix' => '<div id="replace-education">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['education'])) {
            $education = unserialize($result['education']);
            if (count($education) != 0) {
                $index = 0;

                foreach ($education as $key => $value) {
                    create_education($form, $index, $value);
                    $index++;
                }

            } else {
                create_education($form, 0, null);
            }

        } else {
            create_education($form, 0, null);
        }
    } else {
        foreach ($form_state['values']['education'] as $i => $value) {
            create_education($form, $i, null);
        }

        if ($form_state['clicked_button']['#name'] == 'education_add') {
            $last = count($form_state['values']['education']);
            create_education($form, $last, null);
        }
    }

    $form['education']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'education_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_education',
            'wrapper' => 'replace-education',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    //Bereich für Sprachkenntnisse ist analog zu den praktischen Erfahrungen
    $form['language_skills'] = array(
        '#type' => 'fieldset',
        '#title' => t('Sprachkenntnisse'),
        '#prefix' => '<div id="replace-language-skills">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['language_skills'])) {
            $languageSkills = unserialize($result['language_skills']);
            if (count($languageSkills) != 0) {
                $index = 0;

                foreach ($languageSkills as $key => $value) {
                    create_language_skill($form, $index, $value);
                    $index++;
                }

            } else {
                create_language_skill($form, 0, null);
            }

        } else {
            create_language_skill($form, 0, null);
        }
    } else {

        foreach ($form_state['values']['language_skills'] as $i => $value) {
            create_language_skill($form, $i, null);
        }

        if ($form_state['clicked_button']['#name'] == 'language_skills_add') {
            $last = count($form_state['values']['language_skills']);
            create_language_skill($form, $last, null);
        }
    }

    $form['language_skills']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'language_skills_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_language_skills',
            'wrapper' => 'replace-language-skills',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    //Bereich für weitere Fähigkeiten ist analog zu den praktischen Erfahrungen
    $form['special_knowledge'] = array(
        '#type' => 'fieldset',
        '#title' => t('Besondere Kenntnisse/Fähigkeiten'),
        '#prefix' => '<div id="replace-special-knowledge">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['special_knowledge'])) {
            $specialKnowledge = unserialize($result['special_knowledge']);
            if (count($specialKnowledge) != 0) {
                $index = 0;

                foreach ($specialKnowledge as $key => $value) {
                    create_special_knowledge($form, $index, $value);
                    $index++;
                }

            } else {
                create_special_knowledge($form, 0, null);
            }

        } else {
            create_special_knowledge($form, 0, null);
        }
    } else {

        foreach ($form_state['values']['special_knowledge'] as $i => $value) {
            create_special_knowledge($form, $i, null);
        }

        if ($form_state['clicked_button']['#name'] == 'special_knowledge_add') {
            $last = count($form_state['values']['special_knowledge']);
            create_special_knowledge($form, $last, null);
        }
    }

    $form['special_knowledge']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'special_knowledge_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_special_knowledge',
            'wrapper' => 'replace-special-knowledge',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    //Button zum Speichern ins Array laden
    $form['save_button'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
        '#submit' => array('save_button_submit'),
        '#prefix' => '<br>',
    );

    //Form zurückgeben, die von der Funktion drupal_get_form benötigt wird und auf der Editierseite ausgegeben wird
    return $form;

}

//Funktion zum Aufbauen der Felder für praktischen Erfahrungen
function create_practical_experience(&$form, $row, $defaultValues) {

    //Fieldset hinzufügen, damit alle Felder einer praktischen Erfahrung noch einen Rahmen haben
    $form['practical_experience'][$row]['wrapper_start'] = array(
        '#markup' => '<fieldset><div class="fieldset-wrapper">'
    );

    $form['practical_experience'][$row]['activity_input'] = array(
        '#type' => 'textfield',
        '#title' => t('Tätigkeit'),
        '#default_value' => (isset($defaultValues['activity_input'])) ? $defaultValues['activity_input'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'

    );

    $form['practical_experience'][$row]['employment_select'] = array(
        '#type' => 'select',
        '#title' => t('Beschäftigungsart'),
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Aushilfsjob/Studentenjob') => t('Aushilfsjob/Studentenjob'),  
            t('Berufsausbildung') => t('Berufsausbildung'),
            t('Meisterausbildung') => t('Meisterausbildung'),
            t('Praktikum') => t('Praktikum'), 
            t('Teilzeitanstellung') => t('Teilzeitanstellung'),
            t('Vollzeitanstellung') => t('Vollzeitanstellung'),
            t('Werksstudent') => t('Werksstudent'),
        ),
        '#default_value' => (isset($defaultValues['employment_select'])) ? $defaultValues['employment_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['practical_experience'][$row]['company_input'] = array(
        '#type' => 'textfield',
        '#title' => t('Unternehmen'),
        '#default_value' => (isset($defaultValues['company_input'])) ? $defaultValues['company_input'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'

    );

    $form['practical_experience'][$row]['sector_input'] = array(
        '#type' => 'textfield',
        '#title' => t('Branche'),
        '#default_value' => (isset($defaultValues['sector_input'])) ? $defaultValues['sector_input'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    //Array mit Monaten definieren
    $months = array(
        SELECT_DEFAULT => SELECT_DEFAULT,
        t('Januar') => t('Januar'),
        t('Februar') => t('Februar'),
        t('März') => t('März'),
        t('April') => t('April'),
        t('Mai') => t('Mai'),
        t('Juni') => t('Juni'),
        t('Juli') => t('Juli'),
        t('August') => t('August'),
        t('September') => t('September'),
        t('Oktober') => t('Oktober'),
        t('November') => t('November'),
        t('Dezember') => t('Dezember'),
    );

    //Array mit Jahren definieren
    $years = array( SELECT_DEFAULT => SELECT_DEFAULT);

    for ($i = date("Y"); $i >= 1950; $i--) {
        $years[strval($i)] = strval($i);
    }

    $form['practical_experience'][$row]['from_month_text'] = array(
        '#markup' => '<b>' . t('Von') . ':</b><br>',
    );

    $form['practical_experience'][$row]['from_month_select'] = array(
        '#type' => 'select',
        '#title' => t('Monat'),
        '#options' => $months,
        '#default_value' => (isset($defaultValues['from_month_select']) && !empty($defaultValues['from_month_select'])) ? $defaultValues['from_month_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['practical_experience'][$row]['from_year_select'] = array(
        '#type' => 'select',
        '#title' => t('Jahr'),
        '#options' => $years,
        '#default_value' => (isset($defaultValues['from_year_select']) && !empty($defaultValues['from_year_select'])) ? $defaultValues['from_year_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['practical_experience'][$row]['to_month_text'] = array(
        '#markup' => '<b>' . t('Bis') . ':</b><br>',
    );

    $form['practical_experience'][$row]['to_month_select'] = array(
        '#type' => 'select',
        '#title' => t('Monat'),
        '#options' => $months,
        '#default_value' => (isset($defaultValues['to_month_select']) && !empty($defaultValues['to_month_select'])) ? $defaultValues['to_month_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['practical_experience'][$row]['to_year_select'] = array(
        '#type' => 'select',
        '#title' => t('Jahr'),
        '#options' => $years,
        '#default_value' => (isset($defaultValues['to_year_select']) && !empty($defaultValues['to_year_select'])) ? $defaultValues['to_year_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['practical_experience'][$row]['work_description_input'] = array(
        '#title' => t('Beschreibung der Tätigkeit'),
        '#type' => 'textarea',

        '#resizable' => false,
        '#default_value' => (isset($defaultValues['work_description_input'])) ? $defaultValues['work_description_input'] : '',
    );

    //Ende vom Fieldset
    $form['practical_experience'][$row]['wrapper_end'] = array(
        '#markup' => '</div></fieldset>'
    );
}

//Funktion zum Aufbauen der Felder für Ausbildungen
function create_education(&$form, $row, $defaultValues) {

    $form['education'][$row]['wrapper_start'] = array(
        '#markup' => '<fieldset><div class="fieldset-wrapper">'
    );

    $form['education'][$row]['education_art_select'] = array(
        '#type' => 'select',
        '#title' => t('Ausbildungsart'),
        '#options' => array(
            SELECT_DEFAULT =>SELECT_DEFAULT,
            t('Abitur') => t('Abitur'),
            t('Fachhochschulreife') => t('Fachhochschulreife'),
            t('Mittlerer Schulabschluss') => t('Mittlerer Schulabschluss'),
            t('Hauptschulabschluss') => t('Hauptschulabschluss'),
            t('Berufsschule') => t('Berufsschule'),
            t('Studium, Diplom (DH)') => t('Studium, Diplom (DH)'),
            t('Studium, Diplom (FH)') => t('Studium, Diplom (FH)'),
            t('Studium, Diplom (Univ.)') => t('Studium, Diplom (Univ.)'),
            t('Studium, Bachelor') => t('Studium, Bachelor'),
            t('Studium, Master') => t('Studium, Master'),
            t('Studium, Doktor') => t('Studium, Doktor'),
            t('Studium, Akademiebrief') => t('Studium, Akademiebrief'),
            t('Studium, Magister') => t('Studium, Magister'),
            t('Studium, Meisterschüler') => t('Studium, Meisterschüler'),

        ),
        '#default_value' => (isset($defaultValues['education_art_select']) && !empty($defaultValues['education_art_select'])) ? $defaultValues['education_art_select'] : SELECT_DEFAULT,
    );

    $form['education'][$row]['training_center_input'] = array(
        '#type' => 'textfield',
        '#title' => t('Ausbildungsstätte'),
        '#default_value' => (isset($defaultValues['training_center_input'])) ? $defaultValues['training_center_input'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['education'][$row]['training_place_input'] = array(
        '#type' => 'textfield',
        '#title' => t('Ausbildungsort'),
        '#default_value' => (isset($defaultValues['training_place_input'])) ? $defaultValues['training_place_input'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $months = array(
        SELECT_DEFAULT => SELECT_DEFAULT,
        t('Januar') => t('Januar'),
        t('Februar') => t('Februar'),
        t('März') => t('März'),
        t('April') => t('April'),
        t('Mai') => t('Mai'),
        t('Juni') => t('Juni'),
        t('Juli') => t('Juli'),
        t('August') => t('August'),
        t('September') => t('September'),
        t('Oktober') => t('Oktober'),
        t('November') => t('November'),
        t('Dezember') => t('Dezember'),
    );

    $years = array( SELECT_DEFAULT => SELECT_DEFAULT);

    for ($i = date("Y"); $i >= 1950; $i--) {
        $years[strval($i)] = strval($i);
    }

    $form['education'][$row]['from_month_text'] = array(
        '#markup' => '<b>' . t('Von') . ':</b><br>',
    );

    $form['education'][$row]['from_month_select'] = array(
        '#type' => 'select',
        '#title' => t('Monat'),
        '#options' => $months,
        '#default_value' => (isset($defaultValues['from_month_select']) && !empty($defaultValues['from_month_select'])) ? $defaultValues['from_month_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['education'][$row]['from_year_select'] = array(
        '#type' => 'select',
        '#title' => t('Jahr'),
        '#options' => $years,
        '#default_value' => (isset($defaultValues['from_year_select']) && !empty($defaultValues['from_year_select'])) ? $defaultValues['from_year_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['education'][$row]['to_month_text'] = array(
        '#markup' => '<b>' . t('Bis') . ':</b><br>',
    );

    $form['education'][$row]['to_month_select'] = array(
        '#type' => 'select',
        '#title' => t('Monat'),
        '#options' => $months,
        '#default_value' => (isset($defaultValues['to_month_select']) && !empty($defaultValues['to_month_select'])) ? $defaultValues['to_month_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['education'][$row]['to_year_select'] = array(
        '#type' => 'select',
        '#title' => t('Jahr'),
        '#options' => $years,
        '#default_value' => (isset($defaultValues['to_year_select']) && !empty($defaultValues['to_year_select'])) ? $defaultValues['to_year_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['education'][$row]['education_description_input'] = array(
        '#title' => t('Zusätzliche Informationen zur Ausbildung'),
        '#type' => 'textarea',
        '#resizable' => false,
        '#default_value' => (isset($defaultValues['education_description_input'])) ? $defaultValues['education_description_input'] : '',
    );

    $form['education'][$row]['wrapper_end'] = array(
        '#markup' => '</div></fieldset>'
    );
}

//Funktion zum Aufbauen der Felder für Sprachkenntnisse
function create_language_skill(&$form, $row, $defaultValues) {

    $form['language_skills'][$row]['wrapper_start'] = array(
        '#markup' => '<fieldset><div class="fieldset-wrapper">'
    );

    $form['language_skills'][$row]['language_input'] = array(
        '#type' => 'textfield',
        '#title' => t('Sprache'),
        '#default_value' => (isset($defaultValues['language_input'])) ? $defaultValues['language_input'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['language_skills'][$row]['level_select'] = array(
        '#type' => 'select',
        '#title' => t('Level'),
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Grundkenntnisse') => t('Grundkenntnisse'),
            t('Gut') => t('Gut'),
            t('Fließend') => t('Fließend'),
            t('Muttersprache') => t('Muttersprache'),

        ),
        '#default_value' => (isset($defaultValues['level_select']) && !empty($defaultValues['level_select'])) ? $defaultValues['level_select'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['language_skills'][$row]['wrapper_end'] = array(
        '#markup' => '</div></fieldset>'
    );

}

//Funktion zum Aufbauen der Felder für weiteren Fähigkeiten
function create_special_knowledge(&$form, $row, $defaultValues) {

    $form['special_knowledge'][$row]['knowledge_input'] = array(
        '#type' => 'textfield',
        '#default_value' => (isset($defaultValues)) ? $defaultValues : '',
        '#attributes' => array(
            'placeholder' => t('z.B. Programmiersprachen (Java, C++, etc.)'),
        ),
    );

}

//Callback Funktionen, die von den "Mehr" Buttons benötigt wird und immer den passenden Teil des Form-Arrays zurückgibt
function callback_practical_experience($form, &$form_state) {
    return $form['practical_experience'];
}

function callback_education($form, &$form_state) {
    return $form['education'];
}

function callback_language_skills($form, &$form_state) {
    return $form['language_skills'];
}

function callback_special_knowledge($form, &$form_state) {
    return $form['special_knowledge'];
}

//Funktion wird aufgerufen, wenn der Button fürs Löschen des Bildes benutzt wird
function delete_image_button_submit($form, &$form_state) {

    //Datei laden
    $file = file_load($form_state['values']['cv_image_id']);

    //Datei löschen
    file_delete($file);

    //Eintrag in der Datenbank anpassen, damit dort nicht noch die alte Image ID steht
    db_update('extended_user_profile_cv')
        ->fields(array(
            'image_id' => 0,
        ))
        ->condition('user_id', $form_state['values']['cv_user_id'], '=')
        ->execute();
}

//Funktion zum Validieren der Form, bevor diese in der DB gespeichert wird
function extended_user_profile_cv_form_validate($form, &$form_state) {

    //Überprüfen, ob in der PLZ nur Zahlen stehen
    if (!empty($form_state['values']['zip_code'])) {
        if (!preg_match('/^[0-9]+$/', $form_state['values']['zip_code'])) {
            form_set_error('zip_code', t('Postleitzahl darf nur Zahlen enthalten.'));
        }
    }

    //Bild in Variable laden und vorher Überprüfen, ob es sich um ein Bild mit dem richtigen Format handelt
    $file = file_save_upload('cv_image_upload', array(
        'file_validate_is_image' => array(),
        'file_validate_extensions' => array('png gif jpg jpeg'),
    ));

    //Wenn die Variable gesetzt ist, das Bild an den richtigen Ort verschieben
    if ($file != null) {
        if ($file) {
            if ($file = file_move($file, 'public://profile_cv_images')) {
                //Status auf permanent setzen, sonst wird es von Drupal nach 6 Stunden wieder gelöscht
                $file->status = FILE_STATUS_PERMANENT;

                //Dateiinformationen abspeichern und wieder in $form_state schreiben, sodass die submit Funktion auf die Informationen der Datei zugreifen kann
                file_save($file);
                $form_state['values']['cv_image_upload'] = $file;
            } else {
                form_set_error('file', t('Bild konnte nicht geschrieben werden.'));
            }
        } else {
            form_set_error('file', t('Kein Bild wurde hochgeladen.'));
        }
    }

}

//Funktion zum Abspeichern der Informationen in der DB
function save_button_submit($form, &$form_state) {

    $practicalExperience = array();

    //Alle praktischen Erfahrungen iterieren
    for ($i = 0; $i < count($form_state['values']['practical_experience']); $i++) {

        //Überprüfen, ob an der Stelle $i im Array etwas existiert
        if (isset($form_state['values']['practical_experience'][$i])) {
            $value = $form_state['values']['practical_experience'][$i];

            //Wenn ein Bereich für praktische Erfahrung nachgeladen wird, soll vor dem Speichern überprüft werden, ob in dem Bereich überhaupt
            //mindestens ein Element mit Inhalt existiert
            if(!(empty($value['activity_input']) && empty($value['company_input']) && empty($value['sector_input'])
               && empty($value['work_description_input']) && $value['employment_select'] == SELECT_DEFAULT && $value['from_month_select'] == SELECT_DEFAULT
               && $value['from_year_select'] == SELECT_DEFAULT && $value['to_month_select'] == SELECT_DEFAULT && $value['to_year_select'] == SELECT_DEFAULT)) {

                //Array mit Informationen hinzufügen
                array_push($practicalExperience, array(
                    'activity_input' => $value['activity_input'],
                    'company_input' => $value['company_input'],
                    'sector_input' => $value['sector_input'],
                    'work_description_input' => $value['work_description_input'],

                    'employment_select' => ($value['employment_select'] != SELECT_DEFAULT) ? $value['employment_select']: '',
                    'from_month_select' => ($value['from_month_select'] != SELECT_DEFAULT) ? $value['from_month_select']: '',
                    'from_year_select' => ($value['from_year_select'] != SELECT_DEFAULT) ? $value['from_year_select']: '',
                    'to_month_select' => ($value['to_month_select'] != SELECT_DEFAULT) ? $value['to_month_select']: '',
                    'to_year_select' => ($value['to_year_select'] != SELECT_DEFAULT) ? $value['to_year_select']: '',

                ));
            }
        }
    }

    $education = array();

    //Alle Ausbildungen iterieren. Analog zu praktische Erfahrungen
    for ($i = 0; $i < count($form_state['values']['education']); $i++) {
        if (isset($form_state['values']['education'][$i])) {
            $value = $form_state['values']['education'][$i];

            if(!(empty($value['training_center_input']) && empty($value['training_place_input'])
                && empty($value['education_description_input'])&& $value['education_art_select'] == SELECT_DEFAULT
                && $value['from_month_select'] == SELECT_DEFAULT && $value['from_year_select'] == SELECT_DEFAULT
                && $value['to_month_select'] == SELECT_DEFAULT && $value['to_year_select'] == SELECT_DEFAULT)) {

                array_push($education, array(
                    'training_center_input' => $value['training_center_input'],
                    'training_place_input' => $value['training_place_input'],
                    'education_description_input' => $value['education_description_input'],

                    'education_art_select' => ($value['education_art_select'] != SELECT_DEFAULT) ? $value['education_art_select']: '',
                    'from_month_select' => ($value['from_month_select'] != SELECT_DEFAULT) ? $value['from_month_select']: '',
                    'from_year_select' => ($value['from_year_select'] != SELECT_DEFAULT) ? $value['from_year_select']: '',
                    'to_month_select' => ($value['to_month_select'] != SELECT_DEFAULT) ? $value['to_month_select']: '',
                    'to_year_select' => ($value['to_year_select'] != SELECT_DEFAULT) ? $value['to_year_select']: '',

                ));
            }
        }
    }

    $languageSkills = array();

    //Alle Sprachkenntnisse iterieren. Analog zu praktische Erfahrungen
    for ($i = 0; $i < count($form_state['values']['language_skills']); $i++) {
        if (isset($form_state['values']['language_skills'][$i])) {
            $value = $form_state['values']['language_skills'][$i];

            if(!(empty($value['language_input']) && $value['level_select'] == SELECT_DEFAULT)) {

                array_push($languageSkills, array(
                    'language_input' => $value['language_input'],

                    'level_select' => ($value['level_select'] != SELECT_DEFAULT) ? $value['level_select']: '',

                ));
            }
        }
    }

    $specialKnowledge = array();

    //Alle weitere Fähigkeiten iterieren. Analog zu praktische Erfahrungen
    for ($i = 0; $i < count($form_state['values']['special_knowledge']); $i++) {
        if (isset($form_state['values']['special_knowledge'][$i])) {
            $value = $form_state['values']['special_knowledge'][$i];

            if(!empty($value['knowledge_input'])) {

                array_push($specialKnowledge, $value['knowledge_input']);
            }
        }
    }

    //Standardmäßig image ID auf 0 setzen
    $imageID = 0;

    //Wenn ein Bild bereits existiert, dann steht die ID in cv_image_id drin und wird in $imageID geladen
    if (isset($form_state['values']['cv_image_id'])) {
        $imageID = $form_state['values']['cv_image_id'];
    }

    //Wenn ein Bild neu hochgeladen wurde, dann soll diese ID abgespeichert werden
    if (isset($form_state['values']['cv_image_upload']->fid)) {
        $imageID = $form_state['values']['cv_image_upload']->fid;

        //Bei einem neuen Bild soll das alte Bild, falls vorhanden, vorher gelöscht werden
        if (isset($form_state['values']['cv_image_id'])) {
            $file = file_load($form_state['values']['cv_image_id']);

            file_delete($file);
        }
    }

    //User ID auslesen
    $userID = $form_state['values']['cv_user_id'];

    //Überprüfen, ob für den Benutzer bereits ein Eintrag in der Tabelle existiert
    $result = db_select('extended_user_profile_cv', 'ex')
        ->fields('ex', array('user_id'))
        ->condition('user_id', $userID, '=')
        ->execute();

    //Existiert noch kein Eintrag, dann einen neuen Erstellen
    if (!$result->fetchAssoc()) {
        db_insert('extended_user_profile_cv')
            ->fields(array(
                'user_id' => $userID,
                'image_id' => $imageID,
                'salutation' => ($form_state['values']['salutation'] != SELECT_DEFAULT) ? $form_state['values']['salutation'] : '',
                'title' => $form_state['values']['title'],
                'first_name' => $form_state['values']['firstname'],
                'last_name' => $form_state['values']['lastname'],
                'birthday_day' => ($form_state['values']['birthday_day'] != SELECT_DEFAULT) ? $form_state['values']['birthday_day'] : 0,
                'birthday_month' => ($form_state['values']['birthday_month'] != SELECT_DEFAULT) ? $form_state['values']['birthday_month'] : '',
                'birthday_year' => ($form_state['values']['birthday_year'] != SELECT_DEFAULT) ? $form_state['values']['birthday_year'] : 0,
                'email' => $form_state['values']['email'],
                'street' => $form_state['values']['street'],
                'city' => $form_state['values']['city'],
                'zip_code' => !empty($form_state['values']['zip_code']) ? $form_state['values']['zip_code'] : 0,
                'telephone' => $form_state['values']['telephone'],
                'nationality' => $form_state['values']['nationality'],
                'family' => ($form_state['values']['family'] != SELECT_DEFAULT) ? $form_state['values']['family'] : '',
                'practical_experience' => serialize($practicalExperience),
                'education' => serialize($education),
                'language_skills' => serialize($languageSkills),
                'special_knowledge' => serialize($specialKnowledge),
            ))
            ->execute();
        drupal_set_message(t('Informationen wurden gespeichert.'));


    } else {
        //Existiert bereits ein Eintrag, dann soll dieser aktualisiert werden
        db_update('extended_user_profile_cv')
            ->fields(array(
                'image_id' => $imageID,
                'salutation' => ($form_state['values']['salutation'] != SELECT_DEFAULT) ? $form_state['values']['salutation'] : '',
                'title' => $form_state['values']['title'],
                'first_name' => $form_state['values']['firstname'],
                'last_name' => $form_state['values']['lastname'],
                'birthday_day' => ($form_state['values']['birthday_day'] != SELECT_DEFAULT) ? $form_state['values']['birthday_day'] : 0,
                'birthday_month' => ($form_state['values']['birthday_month'] != SELECT_DEFAULT) ? $form_state['values']['birthday_month'] : '',
                'birthday_year' => ($form_state['values']['birthday_year'] != SELECT_DEFAULT) ? $form_state['values']['birthday_year'] : 0,
                'email' => $form_state['values']['email'],
                'street' => $form_state['values']['street'],
                'city' => $form_state['values']['city'],
                'zip_code' => !empty($form_state['values']['zip_code']) ? $form_state['values']['zip_code'] : 0,
                'telephone' => $form_state['values']['telephone'],
                'nationality' => $form_state['values']['nationality'],
                'family' => ($form_state['values']['family'] != SELECT_DEFAULT) ? $form_state['values']['family'] : '',
                'practical_experience' => serialize($practicalExperience),
                'education' => serialize($education),
                'language_skills' => serialize($languageSkills),
                'special_knowledge' => serialize($specialKnowledge),
            ))
            ->condition('user_id', $userID, '=')
            ->execute();

        drupal_set_message(t('Informationen wurden geändert.'));
    }
}
