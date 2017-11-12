<?php

function display_userprofile($user_id) {

    //Überprüfen, ob Benutzer mit der ID existiert
    $userLoad = user_load($user_id);
    if (!$userLoad) {
        drupal_not_found();
        drupal_exit();
    }

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    //Aktuell angemeldeten Benutzer aus globalem $user laden
    global $user;
    $content = array();

    //Felder für die Datenbankabfrage
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

    //Daten aus der DB laden
    $result = db_select('extended_user_profile', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    //HTML für die Tabs der vier Bereiche
    $tabsHTML =
        '<div class="tab">' .
        '<a class="tablinks active" target="general">' . t('Allgemein') . '</a>' .
        '<a class="tablinks" target="contact-preference">' . t('Kontaktpräferenz') . '</a>' .
        '<a class="tablinks" target="personal">' . t('Persönlich') . '</a>' .
        '<a class="tablinks" target="others">' . t('Sonstiges') . '</a>' .
        '</div>';

    //Array $content wird nach und nach mit HTML Inhalt befüllt und zum Schluss zurückgegeben, sodass Drupal den Inhalt des Arrays dann anzeigt
    $content['tabarea'] = array(
        '#markup' => $tabsHTML,
    );

    //Jeder Inhalt eines Tabs muss in tabcontent stehen
    $content['general_start'] = array(
        '#markup' => '<div id="general" class="tabcontent">',
    );


    //Nutzerprofil Allgemein
    $content['general_headline'] = array(
        '#markup' => '<h3>' . t('Allgemein & Kontaktdaten') . '</h3>',
    );

    //Falls existiert, Bild laden und HTML erzeugen
    $imageText = '';
    if ($result['image_id'] != 0) {
        $image = file_load($result['image_id']);


        $variables = array(
            'path' => $image->uri,
            'alt' => t('Profilbild'),
            'title' => t('Profilbild'),
            'width' => '200px',
        );
        $imageText = theme('image', $variables);

    } else {
        $imageText = '<div class="picture-alternative">' . t('Kein Profilbild angegeben.') . '</div>';
    }

    $content['general_left_container'] = array(
        '#markup' => '<div class="general-left-container">' . $imageText . '</div>'
    );

    //Jugend Forscht Projekte aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $jfProjectsHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['jufo_projects'])) {
        $jfProjectsHTML = '';
        $jfProjects = unserialize($result['jufo_projects']);

        for ($i = 0; $i < count($jfProjects); $i++) {
            $jfProjectsHTML .= '<span class="profile-value">' . $jfProjects[$i] . '</span><br>';
        }

        if (count($jfProjects) == 0) {
            $jfProjectsHTML = NO_INDICATION_DEFAULT;
        }

    }

    //Andere Projekte aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $otherProjectsHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['other_projects'])) {
        $otherProjectsHTML = '';
        $otherProjects = unserialize($result['other_projects']);

        for ($i = 0; $i < count($otherProjects); $i++) {
            $otherProjectsHTML .= '<span class="profile-value">' . $otherProjects[$i] . '</span><br>';
        }

        if (count($otherProjects) == 0) {
            $otherProjectsHTML = NO_INDICATION_DEFAULT;
        }

    }

    //Besuchte Schulen aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $visitedSchoolsHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['visited_schools'])) {
        $visitedSchoolsHTML = '';
        $visitedSchools = unserialize($result['visited_schools']);

        for ($i = 0; $i < count($visitedSchools); $i++) {
            $visitedSchoolsHTML .= '<span class="profile-value">' . $visitedSchools[$i] . '</span><br>';
        }

        if (count($visitedSchools) == 0) {
            $visitedSchoolsHTML = NO_INDICATION_DEFAULT;
        }
    }

    //Benutzername zusammenbauen
    $username = (empty($result['first_name']) && empty($result['last_name'])) ? NO_INDICATION_DEFAULT : $result['first_name'] . ' ' . $result['last_name'];

    //HTML Code für die mittlere Spalte im Tab Allgemein fertig stellen
    $HTML =
        '<div id="sep1" class="v-separator"></div>' .
        '<div class="general-middle-container">' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Name') . ':</span><br>' .
        '<span class="profile-value">' . $username . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Jugend Forscht Projekte') . ':</span><br>' .
        $jfProjectsHTML .
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Weitere Projekte') . ':</span><br>' .
        $otherProjectsHTML .
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Besuchte Schulen') . ':</span><br>' .
        $visitedSchoolsHTML .
        '</p>' .
        '</div>';

    $content['general_middle_container'] = array(
        '#markup' => $HTML,
    );

    //Stadt, PLZ und Telefon auf Inhalt prüfen
    $city = (empty($result['city'])) ? NO_INDICATION_DEFAULT : $result['city'];
    $zip_code = (isset($result['zip_code']) && $result['zip_code'] != 0) ? $result['zip_code'] : NO_INDICATION_DEFAULT;
    $telephone = (empty($result['telephone'])) ? NO_INDICATION_DEFAULT : $result['telephone'];

    //Andere Profile aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $otherProfilesHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['other_profiles'])) {
        $otherProfilesHTML = '';
        $otherProfiles = unserialize($result['other_profiles']);

        foreach ($otherProfiles as $key => $value) {
            $otherProfilesHTML .= '<span class="profile-value"><a href="' . $value['portal_link'] . '" target="_blank">' . $value['choose_portal'] . '</a></span><br>';
        }

        if (count($otherProfiles) == 0) {
            $otherProfilesHTML = NO_INDICATION_DEFAULT;
        }
    }

    //HTML Code für die rechte Spalte im Tab Allgemein fertig stellen
    $HTML =
        '<div id="sep2" class="v-separator"></div>' .
        '<div class="general-right-container">' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Stadt') . ':</span><br>' .
        '<span class="profile-value">' . $city . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('PLZ') . ':</span><br>' .
        '<span class="profile-value">' . $zip_code . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Telefon/Mobil') . ':</span><br>' .
        '<span class="profile-value">' . $telephone . '</span>'.
        '</p>' .
        '<p class="profile-container-general">' .
        '<span class="profile-bold-text">' . t('Profile') . ':</span><br>' .
        $otherProfilesHTML .
        '</p>' .
        '</div>';

    $content['general_right_container'] = array(
        '#markup' => $HTML,
    );

    $content['general_end'] = array(
        '#markup' => '<div class="clear-float"></div></div>',
    );

    //Nutzerprofil Kontaktpräferenz

    $content['contactpreference_start'] = array(
        '#markup' => '<div id="contact-preference" class="tabcontent">',
    );

    //Inhalt für die Checkboxen überprüfen und anschließend den HTML Code zusammenbauen
    $checkEmail = ($result['check_email'] == 0) ? '' : '<span class="profile-value">' . t('E-Mail'). '</span><br>';
    $checkPost = ($result['check_post'] == 0) ? '' : '<span class="profile-value">' . t('Post'). '</span><br>';
    $checkTelephone = ($result['check_telephone'] == 0) ? '' : '<span class="profile-value">' . t('Telefon/Mobil'). '</span><br>';
    $checkAll = ($result['check_email'] == 0 && $result['check_post'] == 0 && $result['check_telephone'] == 0) ? t('- keine Angaben -') : '';

    //HTML für den zweiten Tab Kontaktpräferenz zusammenbauen
    $HTML =
        '<span class="profile-bold-text">' . t('Benutzer darf über folgende Kanäle kontaktiert werden') . ':</span>' .
        '<p class="profile-container-contact">' .
        $checkAll .
        $checkEmail .
        $checkPost .
        $checkTelephone .
        '</p>';

    $content['contact_container'] = array(
        '#markup' => $HTML,
    );

    $content['contactpreference_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Persönlich
    $content['personal_start'] = array(
        '#markup' => '<div id="personal" class="tabcontent">',
    );

    //Daten vom Geburtstag prüfen
    $birthdayDay = ($result['birthday_day'] != 0 && !empty($result['birthday_day'])) ? $result['birthday_day'] : '';
    $birthdayMonth = (!empty($result['birthday_month'])) ? $birthdayMonth = $result['birthday_month'] : '';
    $birthdayYear = ($result['birthday_year'] != 0 && !empty($result['birthday_year'])) ? $birthdayYear = $result['birthday_year'] : '';

    $birthday = '';

    //Wenn alle drei leer sind, dann wird keine Angabe angezeigt
    if (empty($birthdayDay) && empty($birthdayMonth) &&  empty($birthdayYear)) {
        $birthday = NO_INDICATION_DEFAULT;
    } else {
        //Andernfalls je nachdem ob der Inhalt leer ist oder nicht, wird das Geburtsdatum zusammengefügt
        $birthday .= (!empty($birthdayDay)) ? $birthdayDay . ' ' : '';
        $birthday .= (!empty($birthdayMonth)) ? $birthdayMonth . ' ' : '';
        $birthday .= (!empty($birthdayYear)) ? $birthdayYear . ' ' : '';
    }

    //Inhalt von Geschlecht überprüfen
    $gender = ($result['gender'] == null) ? NO_INDICATION_DEFAULT : $result['gender'];

    //Hobbies aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $hobbiesHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['hobbies'])) {
        $hobbiesHTML = '';
        $hobbies = unserialize($result['hobbies']);

        foreach ($hobbies as $hobbie) {
            $hobbiesHTML .= '<span class="profile-value">' . $hobbie . '</a></span><br>';
        }

        if (count($hobbies) == 0) {
            $hobbiesHTML = NO_INDICATION_DEFAULT;
        }
    }

    //Interessen aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $interestsHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['interests'])) {
        $interestsHTML = '';
        $interests = unserialize($result['interests']);

        foreach ($interests as $interest) {
            $interestsHTML .= '<span class="profile-value">' . $interest . '</a></span><br>';
        }

        if (count($interests) == 0) {
            $interestsHTML = NO_INDICATION_DEFAULT;
        }
    }

    //HTML für den dritten Tab Persönlich zusammenbauen
    $HTML =
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Geburtsdatum') . ':</span><br>' .
        '<span class="profile-value">' . $birthday . '</span>'.
        '</p>' .
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Geschlecht') . ':</span><br>' .
        '<span class="profile-value">' . $gender . '</span>'.
        '</p>' .
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Hobbys') . ':</span><br>' .
        $hobbiesHTML .
        '</p>'.
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Interessen') . ':</span><br>' .
        $interestsHTML .
        '</p>';

    $content['personal_container'] = array(
        '#markup' => $HTML,
    );

    $content['personal_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Others
    $content['others_start'] = array(
        '#markup' => '<div id="others" class="tabcontent">',
    );

    //Auszeichnungen aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $awardsHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['interests'])) {
        $awardsHTML = '';
        $awards = unserialize($result['received_awards']);

        foreach ($awards as $award) {
            $awardsHTML .= '<span class="profile-value">' . $award . '</a></span><br>';
        }

        if (count($awards) == 0) {
            $awardsHTML = NO_INDICATION_DEFAULT;
        }
    }

    //Organisationen aus der DB laden, String in Array umwandeln, Array iterieren und den HTML Code zusammenbauen
    $organisationsHTML = NO_INDICATION_DEFAULT;

    if (!empty($result['organisations'])) {
        $organisationsHTML = '';
        $organisations = unserialize($result['organisations']);

        foreach ($organisations as $organisation) {
            $organisationsHTML .= '<span class="profile-value">' . $organisation . '</a></span><br>';
        }

        if (count($organisations) == 0) {
            $organisationsHTML = NO_INDICATION_DEFAULT;
        }
    }

    //HTML Code für den vierten Tab Sonstiges zusammenbauen
    $HTML =
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Erhaltene Auszeichnungen') . ':</span><br>' .
        $awardsHTML .
        '</p>'.
        '<p class="profile-container-personal">' .
        '<span class="profile-bold-text">' . t('Organisationen') . ':</span><br>' .
        $organisationsHTML .
        '</p>';

    $content['others_container'] = array(
        '#markup' => $HTML,
    );

    $content['others_end'] = array(
        '#markup' => '</div>',
    );


    //Wenn der Benutzer sein eigenes Profil aufruft oder ein Administrator das Profil aufruft, wird ein Editierbutton angezeigt
    if ($user_id == $user->uid || user_has_role(3, user_load($user->uid))) {
        $content['edit_button'] = array(
            '#markup' => '<br><a class="btn" href="' . base_path() . 'user/' . $user_id . '/userprofile/edit">' . t('Editieren') . '</a>',
        );
    }

    //$content zurückgeben, sodass Drupal diesen anzeigen kann
    return $content;
}