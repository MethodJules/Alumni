<?php

function extended_user_profile_form($form, &$form_state, $user_id) {
    //Überprüfen, ob ein Benutzer mit der ID existiert
    $userLoad = user_load($user_id);
//dvm($userLoad);
    if(!$userLoad){
        drupal_not_found();
        drupal_exit();
    }

    //Aktuell angemeldeten Benutzer aus global $user laden
    global $user;

    //Wenn der Benutzer nicht seine eigene Profilseite aufruft oder es sich nicht um einen Administrator handelt, wird der Zugriff verweigert
    if ($user_id != $user->uid && !user_has_role(3, user_load($user->uid))) {
        drupal_access_denied();
        drupal_exit();
    }

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    //Felder für Datenbankabfrage
    $fields = array(
        'image_id',
        'first_name',
        'last_name',
        'jufo_projects',
        'other_projects',
        'visited_schools',
        'city',
        'zip_code',
        'telephone',
        'other_profiles',
        'check_email',
        'check_post',
        'check_telephone',
        'birthday_day',
        'birthday_month',
        'birthday_year',
        'gender',
        'hobbies',
        'interests',
        'received_awards',
        'organisations'
    );

    //Wenn die ID der aufgerufenen Seite ungleich der ID des aktuell angemeldeten Benutzer ist und der Benutzer den Administratoren angehört, dann bleibt user_id,
    //ansonsten wird user_id die ID des aktuell angemeldeten Benutzers.
    $user_id = ($user_id != $user->uid && user_has_role(3, user_load($user->uid))) ? $user_id : $user->uid;

    //Daten aus der DB laden
    $result = db_select('extended_user_profile', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    //User ID in einem hidden Feld speichern, damit beim Submit die ID zur Verfügung steht
    $form['profile_user_id'] = array(
        '#type' => 'hidden',
        '#value' => $user_id,
    );

    //HTML Code für die Tabs wie beim Anzeigemodus erstellen
    $tabsHTML =
        '<div class="tab">' .
        '<a class="tablinks active" target="general">' . t('Allgemein') . '</a>' .
        '<a class="tablinks" target="contact-preference">' . t('Kontaktpräferenz') . '</a>' .
        '<a class="tablinks" target="personal">' . t('Persönlich') . '</a>' .
        '<a class="tablinks" target="others">' . t('Sonstiges') . '</a>' .
        '</div>';

    $form['tabarea'] = array(
        '#markup' => $tabsHTML,
    );

    $form['general_start'] = array(
        '#markup' => '<div id="general" class="tabcontent">',
    );


    //Nutzerprofil Allgemein

    $form['general_headline'] = array(
        '#markup' => '<h3>' . t('Allgemein') . '</h3>',
    );

    //Überprüfen, ob es Bild bereits für den Benutzer existiert, dieses laden und anzeigen, sowie einen Button zum Löschen anzeigen lassen
    if ($result['image_id'] != 0) {

        $image = file_load($result['image_id']);

        $form['profile_image'] = array(
            '#markup' => '<img src="' . file_create_url($image->uri) . '" width="200px"><br>'
        );

        $form['delete_image'] = array(
            '#type' => 'submit',
            '#value' => t('Bild löschen'),
            '#submit' => array('delete_image_button_submit'),
        );

        $form['profile_image_id'] = array(
            '#type' => 'hidden',
            '#value' => $result['image_id'],
        );
    }

    //Uploadmaske
    $form['profile_image_upload'] = array(
        '#type' => 'file',
        '#title' => t('Profilbild hochladen'),
        '#description' => t('Erlaubte Erweiterungen: jpg, jpeg, png, gif'),
    );

    /*$test = array (

        'parent1' => array(
            3 => 'child1',
            4 => 'child2',
            5 => 'child3',
        ),
        'parent2' => array(
            8 => 'child1',
            9 => 'child2',
        ),
    );

    $form['test'] = array(
        '#type' => 'select',
        '#title' => t('Test'),
        '#options' => $test,
    );*/

    //Vor- und Nachname Eingabefelder
    $form['firstname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Vorname'),
        '#default_value' => (isset($result['first_name']) && !empty($result['first_name'])) ? $result['first_name'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['lastname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Nachname'),
        '#default_value' => (isset($result['last_name']) && !empty($result['last_name'])) ? $result['last_name'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    //Fieldseit für Jugend Forscht Projekte
    $form['jf_projects'] = array(
        '#type' => 'fieldset',
        '#title' => t('Jugend Forscht Projekt auswählen'),
        '#prefix' => '<div id="replace-jf-projects">', //Der Bereich wird beim Nachladen von dynmaischen Feldern ersetzt
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    //Überprüfen ob ein Button geklickt wurde. Passiert beim dynamischen Nachladen von Feldern
    if (!array_key_exists('clicked_button', $form_state)) {

        //Beim Aufruf der Seite überprüfen, ob bereits Daten in der Datenbank stehen
        if (isset($result['jufo_projects'])) {
            //String in Array umwandeln, überprüfen, ob das Array Inhalt hat und anschließend das Array iterieren und
            //die Funktion create_jf_project mit den Daten aus der Datenbank aufrufen. Felder werden mit
            //Daten aus der Datenbank befüllt
            $jfProjects = unserialize($result['jufo_projects']);

            if (count($jfProjects) != 0) {
                for ($i = 0; $i < count($jfProjects); $i++) {
                    create_jf_project($form, $i, t($jfProjects[$i]));

                }
            }else {
                //Andernfalls leere Felder erzeugen
                create_jf_project($form, 0, '');
            }
        } else {
            //Andernfalls leere Felder erzeugen
            create_jf_project($form, 0, '');
        }
    } else {


        // Beim dynamischen Nachladen von Feldern alle bisher existierenden iterieren und die Felder aufbauen
        foreach ($form_state['values']['jf_projects'] as $i => $value) {
            create_jf_project($form, $i, '');
        }

        //Wenn der Button für praktische Erfahrung geklickt wurde, einen zusätzlichen Bereich mit Feldern laden
        if ($form_state['clicked_button']['#name'] == 'jf_projects_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['jf_projects']);
            create_jf_project($form, $last, '');
        }
    }

    //Mehr Button für Jugend Forscht Projekte definieren
    $form['jf_projects']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'jf_projects_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_jf_projects',
            'wrapper' => 'replace-jf-projects',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    //Button für Weiterleitung in die Jugend Forscht Datenbank
    $form['jf_projects']['jf_project_db'] = array(
        '#type' => 'submit',
        '#value' => t('JuFo-Projektdatenbank'),
        '#attributes' => array('onClick' => 'window.open("https://www.jugend-forscht.de/projektdatenbank.html", "_blank");'),
        '#weight' => 1001
    );

    //Button zum Vorschlagen eines Projektes
    $form['jf_projects']['suggest_project'] = array(
        '#type' => 'button',
        '#value' => t('Projekt vorschlagen'),
        '#weight' => 1002
    );


    //Bereich ist analog zu den Jugend Forscht Projekten
    $form['other_projects'] = array(
        '#type' => 'fieldset',
        '#title' => t('Weitere Projekte auswählen'),
        '#prefix' => '<div id="replace-other-projects">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['other_projects'])) {
            $otherProjects = unserialize($result['other_projects']);

            if (count($otherProjects) != 0) {
                for ($i = 0; $i < count($otherProjects); $i++) {
                    create_other_project($form, $i, t($otherProjects[$i]));
                }
            } else {
                create_other_project($form, 0, '');
            }
        }else {
            create_other_project($form, 0, '');
        }
    } else {

        foreach ($form_state['values']['other_projects'] as $i => $value) {
            create_other_project($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'other_projects_add') {
            $last = count($form_state['values']['other_projects']);
            create_other_project($form, $last, '');
        }
    }

    $form['other_projects']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'other_projects_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_other_projects',
            'wrapper' => 'replace-other-projects',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );


    //Bereich ist analog zu den Jugend Forscht Projekten
    $form['visited_schools'] = array(
        '#type' => 'fieldset',
        '#title' => t('Besuchte Schule auswählen'),
        '#prefix' => '<div id="replace-visited-schools">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['visited_schools'])) {
            $visitedSchools = unserialize($result['visited_schools']);

            if (count($visitedSchools) != 0) {
                for ($i = 0; $i < count($visitedSchools); $i++) {
                    create_visited_school($form, $i, t($visitedSchools[$i]));
                }
            } else {
                create_visited_school($form, 0, '');
            }
        } else {
            create_visited_school($form, 0, '');
        }
    } else {

        foreach ($form_state['values']['visited_schools'] as $i => $value) {
            create_visited_school($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'visited_schools_add') {
            $last = count($form_state['values']['visited_schools']);
            create_visited_school($form, $last, '');
        }
    }

    $form['visited_schools']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'visited_schools_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_visited_schools',
            'wrapper' => 'replace-visited-schools',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['visited_schools']['suggest_school'] = array(
        '#type' => 'submit',
        '#value' => t('Schule vorschlagen'),
        '#submit' => array('suggest_school_submit'),
        '#weight' => 1001
    );

    //Nutzerprofil Kontaktdaten

    $form['contactdetails_headline'] = array(
        '#markup' => '<br><hr><br><h3>' . t('Kontaktdaten') . '</h3>',
    );

    $form['city'] = array(
        '#type' => 'textfield',
        '#title' => t('Wohnort'),
        '#default_value' => (isset($result['city']) && !empty($result['city'])) ? $result['city'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['zip_code'] = array(
        '#type' => 'textfield',
        '#title' => t('PLZ'),
        '#default_value' => (isset($result['zip_code']) && $result['zip_code'] != 0) ? $result['zip_code'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['telephone'] = array(
        '#type' => 'textfield',
        '#title' => t('Telefon/Mobil'),
        '#default_value' => (isset($result['telephone']) && !empty($result['telephone'])) ? $result['telephone'] : '',
    );

    //Bereich ist analog zu den Jugend Forscht Projekten
    $form['other_profiles'] = array(
        '#type' => 'fieldset',
        '#title' => t('Weitere Profile auswählen'),
        '#prefix' => '<div id="replace-other-profiles">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['other_profiles'])) {
            $otherProfiles = unserialize($result['other_profiles']);
            if (count($otherProfiles) != 0) {
                $index = 0;

                for ($i = 0; $i < count($otherProfiles); $i++) {
                    create_other_profile($form, $i, $otherProfiles[$i]);
                    $index++;
                }

            } else {
                create_other_profile($form, 0, null);
            }

        } else {
            create_other_profile($form, 0, null);
        }
    } else {
        foreach ($form_state['values']['other_profiles'] as $i => $value) {
            create_other_profile($form, $i, null);
        }

        if ($form_state['clicked_button']['#name'] == 'other_profiles_add') {
            $last = count($form_state['values']['other_profiles']);
            create_other_profile($form, $last, null);
        }
    }

    $form['other_profiles']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'other_profiles_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_other_profiles',
            'wrapper' => 'replace-other-profiles',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['general_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Kontaktpräferenz

    $form['contactpreference_start'] = array(
        '#markup' => '<div id="contact-preference" class="tabcontent">',
    );

    $form['contactpreference_headline'] = array(
        '#markup' => t('Über folgende Kanäle darf Kontakt zu mir aufgenommen werden'),
    );

    $form['check_email'] = array(
        '#type' => 'checkbox',
        '#title' => t('E-Mail'),
        '#default_value' => (isset($result['check_email'])) ? $result['check_email'] : 0,
    );

    $form['check_post'] = array(
        '#type' => 'checkbox',
        '#title' => t('Post'),
        '#default_value' => (isset($result['check_post'])) ? $result['check_post'] : 0,
    );

    $form['check_telephone'] = array(
        '#type' => 'checkbox',
        '#title' => t('Telefon/Mobil'),
        '#default_value' => (isset($result['check_telephone'])) ? $result['check_telephone'] : 0,
    );

    $form['contactpreference_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Persönlich
    $form['personal_start'] = array(
        '#markup' => '<div id="personal" class="tabcontent">',
    );

    //Arrays mit Tagen, Monaten und Jahren definieren
    $days = array(SELECT_DEFAULT => SELECT_DEFAULT);
    for ($i = 1; $i <= 31; $i++) {
        $days[strval($i)] = strval($i);
    }

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

    $years = array(SELECT_DEFAULT => SELECT_DEFAULT);

    for ($i = date("Y"); $i >= 1950; $i--) {
        $years[strval($i)] = strval($i);
    }

    $form['birthday'] = array(
        '#markup' => '<b>' . t('Geburtstag') . ':</b><br>',
    );

    $form['birthday_day'] = array(
        '#type' => 'select',
        '#title' => t('Tag'),
        '#options' => $days,
        '#default_value' => (isset($result['birthday_day']) && $result['birthday_day'] != 0) ? $result['birthday_day'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['birthday_month'] = array(
        '#type' => 'select',
        '#title' => t('Monat'),
        '#options' => $months,
        '#default_value' => (isset($result['birthday_month']) && !empty($result['birthday_month'])) ? $result['birthday_month'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['birthday_year'] = array(
        '#type' => 'select',
        '#title' => t('Jahr'),
        '#options' => $years,
        '#default_value' => (isset($result['birthday_year']) && $result['birthday_year'] != 0) ? $result['birthday_year'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>'
    );

    $active = array(t('männlich') => t('männlich'), t('weiblich') => t('weiblich'));
    $form['gender'] = array(
        '#type' => 'radios',
        '#title' => t('Geschlecht'),
        '#options' => $active,
        '#default_value' => (isset($result['gender']) && $result['gender'] != null) ? $result['gender'] : '',
    );


    //Bereich ist analog zu den Jugend Forscht Projekten
    $form['hobbies'] = array(
        '#type' => 'fieldset',
        '#title' => t('Hobbys'),
        '#prefix' => '<div id="replace-hobbies">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['hobbies'])) {
            $hobbies = unserialize($result['hobbies']);

            if (count($hobbies) != 0) {
                for ($i = 0; $i < count($hobbies); $i++) {
                    create_hobbie($form, $i, t($hobbies[$i]));
                }
            } else {
                create_hobbie($form, 0, '');
            }
        } else {
            create_hobbie($form, 0, '');
        }
    } else {
        foreach ($form_state['values']['hobbies'] as $i => $value) {
            create_hobbie($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'hobbies_add') {
            $last = count($form_state['values']['hobbies']);
            create_hobbie($form, $last, '');
        }
    }

    $form['hobbies']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'hobbies_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_hobbies',
            'wrapper' => 'replace-hobbies',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    //Bereich ist analog zu den Jugend Forscht Projekten
    $form['interests'] = array(
        '#type' => 'fieldset',
        '#title' => t('Interessen'),
        '#prefix' => '<div id="replace-interests">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    $form['interests']['info_button'] = array(
        '#markup' => '<img class="info-box-button" src="' . base_path() . drupal_get_path('module', 'extended_user_profile') . '/img/info_button.png' . '"></a>',
    );

    $form['interests']['info_box'] = array(
        '#markup' => '<div class="info-box">' . variable_get('eup_interest_info_text', '')['value'] . '</div>'
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['interests'])) {
            $interests = unserialize($result['interests']);

            if (count($interests) != 0) {
                for ($i = 0; $i < count($interests); $i++) {
                    create_interest($form, $i, t($interests[$i]));
                }
            } else {
                create_interest($form, 0, '');
            }
        } else {
            create_interest($form, 0, '');
        }
    } else {
        foreach ($form_state['values']['interests'] as $i => $value) {
            create_interest($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'interests_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['interests']);
            create_interest($form, $last, '');
        }
    }

    $form['interests']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'interests_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_interests',
            'wrapper' => 'replace-interests',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['interests']['suggest_interest'] = array(
        '#type' => 'submit',
        '#value' => t('Interesse vorschlagen'),
        '#submit' => array('suggest_interest_submit'),
        '#weight' => 1001
    );

    $form['personal_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Others
    $form['others_start'] = array(
        '#markup' => '<div id="others" class="tabcontent">',
    );

    //Bereich ist analog zu den Jugend Forscht Projekten
    $form['awards'] = array(
        '#type' => 'fieldset',
        '#title' => t('Erhaltene Auszeichnungen'),
        '#prefix' => '<div id="replace-awards">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['received_awards'])) {
            $awards = unserialize($result['received_awards']);

            if (count($awards) != 0) {
                for ($i = 0; $i < count($awards); $i++) {
                    create_award($form, $i, t($awards[$i]));
                }
            } else {
                create_award($form, 0, '');
            }
        } else {
            create_award($form, 0, '');
        }
    } else {
        foreach ($form_state['values']['awards'] as $i => $value) {
            create_award($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'awards_add') {
            $last = count($form_state['values']['awards']);
            create_award($form, $last, '');
        }
    }

    $form['awards']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'awards_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_awards',
            'wrapper' => 'replace-awards',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    //Bereich ist analog zu den Jugend Forscht Projekten
    $form['organisations'] = array(
        '#type' => 'fieldset',
        '#title' => t('Organisationen'),
        '#prefix' => '<div id="replace-organisations">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['organisations'])) {
            $organisations = unserialize($result['organisations']);

            if (count($organisations) != 0) {
                for ($i = 0; $i < count($organisations); $i++) {
                    create_organisation($form, $i, t($organisations[$i]));
                }
            } else {
                create_organisation($form, 0, '');
            }
        } else {
            create_organisation($form, 0, '');
        }
    } else {
        foreach ($form_state['values']['organisations'] as $i => $value) {
            create_organisation($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'organisations_add') {
            $last = count($form_state['values']['organisations']);
            create_organisation($form, $last, '');
        }
    }

    $form['organisations']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'organisations_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_organisations',
            'wrapper' => 'replace-organisations',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['others_end'] = array(
        '#markup' => '</div>',
    );

    //Button zum Abschicken der Form
    $form['save_button'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
        '#validate' => array('save_button_validate'),
        '#submit' => array('save_button_submit'),
        '#prefix' => '<br>',
    );

    return $form;

}

//Funktion zum Aufbauen der Felder für jugend Forscht Projekte
function create_jf_project(&$form, $row, $defaultValue) {
    //$form['agencies'][$row] = array('#type' => 'fieldset');

    $form['jf_projects'][$row]['jf_projects_select'] = array(
        '#type' => 'select',
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Projekt 1') => t('Projekt 1'),
            t('Projekt 2') => t('Projekt 2'),
            t('Projekt 3') => t('Projekt 3'),
        ),
        '#default_value' => $defaultValue,
    );

}

//Funktion zum Aufbauen der Felder für andere Projekte
function create_other_project(&$form, $row, $defaultValue) {
    $form['other_projects'][$row]['other_projects_input'] = array(
        '#type' => 'textfield',
        '#default_value' => $defaultValue,
    );

}

//Funktion zum Aufbauen der Felder für besuchte Schulen
function create_visited_school(&$form, $row, $defaultValue) {

    $form['visited_schools'][$row]['visited_schools_select'] = array(
        '#type' => 'textfield',
        '#autocomplete_path'=>'visitedschools/autocomplete',
        '#default_value' => $defaultValue,
    );
}

//Funktion zum Aufbauen der Felder für andere Profile
function create_other_profile(&$form, $row, $defaultValues) {

    //Ein Fieldset mit Rahmen soll um die Felder für ein anderes Profil gelegt werden
    $form['other_profiles'][$row]['wrapper_start'] = array(
        '#markup' => '<fieldset><div class="fieldset-wrapper">'
    );

    $form['other_profiles'][$row]['choose_portal'] = array(
        '#type' => 'select',
        '#title' => t('Portal'),
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Facebook') => t('Facebook'),
            t('Google+') => t('Google+'),
            t('Instagram') => t('Instagram'),
            t('LinkedIn') => t('LinkedIn'),
            t('Pinterest') => t('Pinterest'),
            t('Snapchat') => t('Snapchat'),
            t('Tumblr') => t('Tumblr'),
            t('Twitter') => t('Twitter'),
            t('XING') => t('XING'),
        ),
        '#default_value' => (isset($defaultValues['choose_portal']) && !empty($defaultValues['choose_portal'])) ? $defaultValues['choose_portal'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['other_profiles'][$row]['portal_link'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Link zum Profil'),
        '#default_value' => $defaultValues['portal_link'],
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['other_profiles'][$row]['wrapper_end'] = array(
        '#markup' => '</div></fieldset>'
    );
}

//Funktion zum Aufbauen der Felder für Hobbies
function create_hobbie(&$form, $row, $defaultValue) {

    $form['hobbies'][$row]['hobbies_input'] = array(
        '#type' => 'textfield',
        '#default_value' => $defaultValue,
    );
}

//Funktion zum Aufbauen der Felder für Interessen
function create_interest(&$form, $row, $defaultValue) {

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('eup_interest_machine_name'));
    $vid = $vocab->vid;

    $taxonomy = taxonomy_get_tree($vid);
    //dvm($taxonomy);

    $options = array();

    $options[SELECT_DEFAULT] = SELECT_DEFAULT;
    foreach ($taxonomy as $data) {

        if ($data->parents[0] === '0') {
            $children = taxonomy_get_children($data->tid);
            //dvm($children);
            foreach ($children as $child) {
                $options[$data->name][$child->name] = $child->name;
            }
        }
    }

    $form['interests'][$row]['interests_select'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $defaultValue,
    );

}

//Funktion zum Aufbauen der Felder für Auszeichnungen
function create_award(&$form, $row, $defaultValue) {

    $form['awards'][$row]['awards_input'] = array(
        '#type' => 'textfield',
        '#default_value' => $defaultValue,
    );

    $form['awards'][$row]['awards_input']['#attributes']['placeholder'] = t('z.B. eine Auszeichnung für "Jugend Forscht" Projekte erhalten');
}

//Funktion zum Aufbauen der Felder für Organisationen
function create_organisation(&$form, $row, $defaultValue) {

    $form['organisations'][$row]['organisations_input'] = array(
        '#type' => 'textfield',
        '#default_value' => $defaultValue,
    );

    $form['organisations'][$row]['organisations_input']['#attributes']['placeholder'] = t('z.B. Engagement bei der freiwilligen Feuerwehr');
}

//Callback Funktionen, die von den "Mehr" Buttons benötigt wird und immer den passenden Teil des Form-Arrays zurückgibt
function callback_jf_projects($form, &$form_state) {
    return $form['jf_projects'];
}

function callback_other_projects($form, &$form_state) {
    return $form['other_projects'];
}

function callback_visited_schools($form, &$form_state) {
    return $form['visited_schools'];
}

function callback_other_profiles($form, &$form_state) {
    return $form['other_profiles'];
}

function callback_hobbies($form, &$form_state) {
    return $form['hobbies'];
}

function callback_interests($form, &$form_state) {
    return $form['interests'];
}

function callback_awards($form, &$form_state) {
    return $form['awards'];
}

function callback_organisations($form, &$form_state) {
    return $form['organisations'];
}

function visited_schools_autocomplete($string) {

    $stringArr = preg_split('/ /', $string);

    $matches = array();

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('eup_school_machine_name'));
    $vid = $vocab->vid;

    $query = db_select('taxonomy_term_data', 'ttd');
    $query->fields('ttd', array('tid', 'name'));
    $query->join('taxonomy_term_hierarchy','tth' , 'ttd.tid = tth.tid');
    $query->condition('ttd.vid', $vid, '=');

    foreach ($stringArr as $string) {
        if (!empty($string)) {
            $query->condition('ttd.name', '%' . db_like($string) . '%', 'LIKE');
        }
    }
    $query->condition('tth.parent', 0, '!=');
    $query->range(0, 10);
    $result = $query->execute();

    foreach ($result as $term) {
        $matches[check_plain($term -> name)] = check_plain($term -> name);
    }

    drupal_json_output($matches);
}

//Funktion zum Validieren der Form, bevor diese in der DB gespeichert wird
function save_button_validate($form, &$form_state) {

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    //Überprüfen, ob in der PLZ nur Zahlen stehen
    if (!empty($form_state['values']['zip_code'])) {
        if (!preg_match('/^[0-9]+$/', $form_state['values']['zip_code'])) {
            form_set_error('zip_code', t('Postleitzahl darf nur Zahlen enthalten.'));
        }
    }

    //Überprüfen, ob ein gültiger Link bei den Portalen angegeben wurde
    $index = 0;
    foreach ($form_state['values']['other_profiles'] as $value) {
        if (isset($value['portal_link'])) {
            if (!preg_match('/(http|https):\/\//', $value['portal_link']) && !empty($value['portal_link'])) {
                form_set_error('other_profiles][' . $index . '][portal_link', t('Kein gültiger Link beim Portal angegeben.'));
            }
        }
        $index++;
    }

    //Bild in Variable laden und vorher Überprüfen, ob es sich um ein Bild mit dem richtigen Format handelt
    $file = file_save_upload('profile_image_upload', array(
        'file_validate_is_image' => array(),
        'file_validate_extensions' => array('png gif jpg jpeg'),
    ));

    //Wenn die Variable gesetzt ist, das Bild an den richtigen Ort verschieben
    if ($file != null) {
        if ($file) {
            if ($file = file_move($file, 'public://profile_images')) {
                //Status auf permanent setzen, sonst wird es von Drupal nach 6 Stunden wieder gelöscht
                $file->status = FILE_STATUS_PERMANENT;

                //Dateiinformationen abspeichern und wieder in $form_state schreiben, sodass die submit Funktion auf die Informationen der Datei zugreifen kann
                file_save($file);
                $form_state['values']['profile_image_upload'] = $file;

                $userLoad = user_load($form_state['values']['profile_user_id']);
                $userLoad->picture = $file;
                user_save($userLoad);
            } else {
                form_set_error('file', t('Bild konnte nicht geschrieben werden.'));
            }
        } else {
            form_set_error('file', t('Kein Bild wurde hochgeladen.'));
        }
    }

    //Überprüfen, ob die angegebene Schule in der Liste für gültige Schulen steht
    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('eup_school_machine_name'));
    $vid = $vocab->vid;

    $index = 0;
    foreach ($form_state['values']['visited_schools'] as $value) {
        if (!empty($value['visited_schools_select'])) {

            $query = db_select('taxonomy_term_data', 'ttd');
            $query->fields('ttd', array('tid', 'name'));
            $query->condition('vid', $vid, '=');
            $query->condition('name', $value['visited_schools_select'], '=');
            $result = $query->execute();

            if(!$result->fetchAssoc()) {
                form_set_error('visited_schools][' . $index . '][visited_schools_select', t('Angegebene Schule ist ungültig.'));
            }
        }
        $index++;
    }

}

function suggest_school_submit($form, &$form_state) {
    drupal_goto('suggest/school');
}

function suggest_interest_submit($form, &$form_state) {
    drupal_goto('suggest/interest');
}

//Funktion wird aufgerufen, wenn der Button fürs Löschen des Bildes benutzt wird
function delete_image_button_submit($form, &$form_state) {
    //Datei laden
    $file = file_load($form_state['values']['profile_image_id']);

    //Datei löschen
    file_delete($file);

    //Eintrag in der Datenbank anpassen, damit dort nicht noch die alte Image ID steht
    db_update('extended_user_profile')
        ->fields(array(
            'image_id' => 0,
        ))
        ->condition('user_id', $form_state['values']['profile_user_id'], '=')
        ->execute();
}

//Funktion zum Abspeichern der Informationen in der DB
function save_button_submit($form, &$form_state) {

    $jfProjects = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['jf_projects'] as $value) {
        if (isset($value['jf_projects_select'])) {
            if ($value['jf_projects_select'] != SELECT_DEFAULT) {
                array_push($jfProjects, $value['jf_projects_select']);

            }
        }
    }

    $otherProjects = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['other_projects'] as $value) {
        if (isset($value['other_projects_input'])) {
            if (!empty($value['other_projects_input'])) {
                array_push($otherProjects, $value['other_projects_input']);

            }
        }
    }

    $visitedSchools = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['visited_schools'] as $value) {
        if (isset($value['visited_schools_select'])) {
            if (!empty($value['visited_schools_select'])) {
                array_push($visitedSchools, $value['visited_schools_select']);

            }
        }
    }

    $otherProfiles = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['other_profiles'] as $value) {
        if (isset($value['choose_portal'])) {
            if ($value['choose_portal'] != SELECT_DEFAULT && !empty($value['portal_link'])) {
                array_push($otherProfiles, array(
                    'choose_portal' => $value['choose_portal'],
                    'portal_link' => $value['portal_link'],
                ));
            }
        }
    }


    $hobbies = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['hobbies'] as $value) {
        if (isset($value['hobbies_input'])) {
            if (!empty($value['hobbies_input'])) {
                array_push($hobbies, $value['hobbies_input']);

            }
        }
    }

    $interests = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['interests'] as $value) {
        if (isset($value['interests_select'])) {
            if ($value['interests_select'] != SELECT_DEFAULT) {
                array_push($interests, $value['interests_select']);

            }
        }
    }

    $awards = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['awards'] as $value) {
        if (isset($value['awards_input'])) {
            if (!empty($value['awards_input'])) {
                array_push($awards, $value['awards_input']);

            }
        }
    }

    $organisations = array();

    //Alle Felder iterieren, den Inhalt überprüfen, dass dieser nicht leer oder auf "- auswählen -" steht und dem Array hinzufügen
    foreach ($form_state['values']['organisations'] as $value) {
        if (isset($value['organisations_input'])) {
            if (!empty($value['organisations_input'])) {
                array_push($organisations, $value['organisations_input']);

            }
        }
    }

    //Standardmäßig image ID auf 0 setzen
    $imageID = 0;

    //Wenn ein Bild bereits existiert, dann steht die ID in cv_image_id drin und wird in $imageID geladen
    if (isset($form_state['values']['profile_image_id'])) {
        $imageID = $form_state['values']['profile_image_id'];
    }

    //Wenn ein Bild neu hochgeladen wurde, dann soll diese ID abgespeichert werden
    if (isset($form_state['values']['profile_image_upload']->fid)) {
        $imageID = $form_state['values']['profile_image_upload']->fid;

        //Bei einem neuen Bild soll das alte Bild, falls vorhanden, vorher gelöscht werden
        if (isset($form_state['values']['profile_image_id'])) {
            $file = file_load($form_state['values']['profile_image_id']);

            file_delete($file);
        }
    }

    //Überprüfen, ob für den Benutzer bereits ein Eintrag in der Tabelle existiert
    $result = db_select('extended_user_profile', 'ex')
        ->fields('ex', array('user_id'))
        ->condition('user_id', $form_state['values']['profile_user_id'], '=')
        ->execute();

    //Existiert noch kein Eintrag, dann einen neuen Erstellen
    if (!$result->fetchAssoc()) {
        db_insert('extended_user_profile')
            ->fields(array(
                'user_id' => $form_state['values']['profile_user_id'],
                'image_id' => $imageID,
                'first_name' => $form_state['values']['firstname'],
                'last_name' => $form_state['values']['lastname'],
                'jufo_projects' => serialize($jfProjects),
                'other_projects' => serialize($otherProjects),
                'visited_schools' => serialize($visitedSchools),
                'city' => $form_state['values']['city'],
                'zip_code' => !empty($form_state['values']['zip_code']) ? $form_state['values']['zip_code'] : 0,
                'telephone' => $form_state['values']['telephone'],
                'other_profiles' => serialize($otherProfiles),
                'check_email' => !empty($form_state['values']['check_email']) ? $form_state['values']['check_email'] : 0,
                'check_post' => !empty($form_state['values']['check_post']) ? $form_state['values']['check_post'] : 0,
                'check_telephone' => !empty($form_state['values']['check_telephone']) ? $form_state['values']['check_telephone'] : 0,
                'birthday_day' => ($form_state['values']['birthday_day'] != SELECT_DEFAULT) ? $form_state['values']['birthday_day'] : 0,
                'birthday_month' => ($form_state['values']['birthday_month'] != SELECT_DEFAULT) ? $form_state['values']['birthday_month'] : '',
                'birthday_year' => ($form_state['values']['birthday_year'] != SELECT_DEFAULT) ? $form_state['values']['birthday_year'] : 0,
                'gender' => $form_state['values']['gender'],
                'hobbies' => serialize($hobbies),
                'interests' => serialize($interests),
                'received_awards' => serialize($awards),
                'organisations' => serialize($organisations),
            ))
            ->execute();
        drupal_set_message(t('Informationen wurden gespeichert.'));

    } else {
        //Existiert bereits ein Eintrag, dann soll dieser aktualisiert werden
        db_update('extended_user_profile')
            ->fields(array(
                'image_id' => $imageID,
                'first_name' => $form_state['values']['firstname'],
                'last_name' => $form_state['values']['lastname'],
                'jufo_projects' => serialize($jfProjects),
                'other_projects' => serialize($otherProjects),
                'visited_schools' => serialize($visitedSchools),
                'city' => $form_state['values']['city'],
                'zip_code' => !empty($form_state['values']['zip_code']) ? $form_state['values']['zip_code'] : 0,
                'telephone' => $form_state['values']['telephone'],
                'other_profiles' => serialize($otherProfiles),
                'check_email' => !empty($form_state['values']['check_email']) ? $form_state['values']['check_email'] : 0,
                'check_post' => !empty($form_state['values']['check_post']) ? $form_state['values']['check_post'] : 0,
                'check_telephone' => !empty($form_state['values']['check_telephone']) ? $form_state['values']['check_telephone'] : 0,
                'birthday_day' => ($form_state['values']['birthday_day'] != SELECT_DEFAULT) ? $form_state['values']['birthday_day'] : 0,
                'birthday_month' => ($form_state['values']['birthday_month'] != SELECT_DEFAULT) ? $form_state['values']['birthday_month'] : '',
                'birthday_year' => ($form_state['values']['birthday_year'] != SELECT_DEFAULT) ? $form_state['values']['birthday_year'] : 0,
                'gender' => $form_state['values']['gender'],
                'hobbies' => serialize($hobbies),
                'interests' => serialize($interests),
                'received_awards' => serialize($awards),
                'organisations' => serialize($organisations),
            ))
            ->condition('user_id', $form_state['values']['profile_user_id'], '=')
            ->execute();

        drupal_set_message(t('Informationen wurden geändert.'));
    }

}
