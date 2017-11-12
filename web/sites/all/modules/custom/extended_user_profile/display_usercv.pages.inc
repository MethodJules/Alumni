<?php

function display_usercv ($user_id) {

    //Überprüfen, ob der Benutzer mit der ID existiert
    $userLoad = user_load($user_id);
    if (!$userLoad) {
        drupal_not_found();
        drupal_exit();
    }

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    //Aktuellen Benutzer aus globaler Variable holen
    global $user;

    $content = array();

    //Felder, die aus der Tabelle in der Datenbank ausgelesen werden
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

    //Select ausführen
    $result = db_select('extended_user_profile_cv', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    //cv-container umspannt den gesamenten Lebenslauf
    $content['cv']['cv_container_start'] = array(
        '#markup' => '<div class="cv-container">',
    );

    //Eigenen Page Title definieren, da der Title von Drupal ausgeblendet wird, damit dieser in die Erstelleung des PDFs mit eingehen kann
    $content['cv']['headline'] = array(
        '#markup' => '<div class="cv-page-title">' . t('Lebenslauf') . '</div><br>'
    );

    //Überprüfen, ob ein Bild existiert und den HTML Code erstellen
    $imageHTML = '';
    if ($result['image_id'] != 0) {
        $image = file_load($result['image_id']);


        $variables = array(
            'path' => $image->uri,
            'alt' => t('Profilbild'),
            'title' => t('Profilbild'),
            'width' => '160px',
        );
        $imageHTML = theme('image', $variables);

    }

    //Bild ins Array laden
    $content['cv']['picture_container'] = array(
        '#markup' => '<div class="cv-picture-container">' . $imageHTML . '</div>'
    );

    //Überschrift ins Array laden
    $content['cv']['personal_headline'] = array(
        '#markup' => '<div class="cv-headline">' . t('Persönliche Daten') . '</div><br>'
    );

    //HTML für Tabellenelemente in Variablen speichern. Falls mal eine Klasse o.ä. umbenannt werden muss, brauch nur die Variable geändert werden
    $tableStart = '<div class="cv-table">';
    $tableEnd = '</div>';
    $perRowStart = '<div class="cv-row personal-row">';
    $rowStart = '<div class="cv-row">';
    $rowEnd = '</div>';
    $perLeftCellStart = '<div class="cv-cell cell-left personal-cell">';
    $leftCellStart = '<div class="cv-cell cell-left">';
    $rightCellStart = '<div class="cv-cell cell-right">';
    $cellEnd = '</div>';

    $personalHTML = NO_INDICATION_DEFAULT;

    //Überprüfen, ob alle Felder für die persönlichen Daten Inhalt haben. Falls alle Felder leer oder nicht gesetzt sind
    //bleibt der Text "keine Angabe" stehen
    if (!(empty($result['salutation']) && empty($result['title'])
        && empty($result['first_name']) && empty($result['last_name'])
        && $result['birthday_day'] == 0 && empty($result['birthday_month'])
        && $result['birthday_year'] == 0 && empty($result['email'])
        && empty($result['city']) && $result['zip_code'] == 0
        && empty($result['telephone']) && empty($result['nationality'])
        && empty($result['family']))){

        $userName = '';

        //Felder für Name überprüfen und HTML zusammenbauen
        if (!(empty($result['salutation']) && empty($result['title'])
            && empty($result['first_name']) && empty($result['last_name']))) {

            $userName .= $perRowStart.$perLeftCellStart.t('Name').'<br>'.$cellEnd.$rightCellStart;

            !empty($result['salutation']) ? $userName .= $result['salutation'] . ' ' : '';
            !empty($result['title']) ? $userName .= $result['title'] . ' ' : '';
            !empty($result['first_name']) ? $userName .= $result['first_name'] . ' ' : '';
            !empty($result['last_name']) ? $userName .= $result['last_name'] : '';

            $userName .= '<br>'.$cellEnd.$rowEnd;

        }

        $birthday = '';

        //Felder für Geburtstag überprüfen und HTML zusammenbauen
        if (!($result['birthday_day'] == 0 && empty($result['birthday_month'])
            && $result['birthday_year'])) {

            $birthday .= $perRowStart.$perLeftCellStart.t('Geburtsdatum').'<br>'.$cellEnd.$rightCellStart;

            $result['birthday_day'] != 0 ? $birthday .= $result['birthday_day'] . ' ' : '';
            !empty($result['birthday_month']) ? $birthday .= $result['birthday_month'] . ' ' : '';
            $result['birthday_year'] != 0 ? $birthday .= $result['birthday_year'] . '' : '';

            $birthday .= '<br>'.$cellEnd.$rowEnd;
        }

        $address = '';

        //Felder für Adresse überprüfen und HTML zusammenbauen
        if (!(empty($result['city']) && $result['zip_code'] == 0 && empty($result['street']))) {

            $address .= $perRowStart.$perLeftCellStart.t('Adresse').'<br>'.$cellEnd.$rightCellStart;

            !empty($result['street']) ? $address .= $result['street'] . '<br>' : '';
            $result['zip_code'] != 0 ? $address .= $result['zip_code'] . ' ' : '';
            !empty($result['city']) ? $address .= $result['city'] : '';

            $address .= '<br>'.$cellEnd.$rowEnd;
        }

        $email = '';

        //Felder für E-Mail überprüfen und HTML zusammenbauen
        if (!empty($result['email'])) {
            $email .= $perRowStart.$perLeftCellStart.t('E-Mail').'<br>'.$cellEnd.$rightCellStart.$result['email'].'<br>'.$cellEnd.$rowEnd;
        }

        $telephone = '';

        //Felder für Telefon überprüfen und HTML zusammenbauen
        if (!empty($result['telephone'])) {
            $telephone .= $perRowStart.$perLeftCellStart.t('Telefon').'<br>'.$cellEnd.$rightCellStart.$result['telephone'].'<br>'.$cellEnd.$rowEnd;
        }

        $nationality = '';

        //Felder für Nationalität überprüfen und HTML zusammenbauen
        if (!empty($result['nationality'])) {
            $nationality .= $perRowStart.$perLeftCellStart.t('Staatsangehörigkeit').'<br>'.$cellEnd.$rightCellStart.$result['nationality'].'<br>'.$cellEnd.$rowEnd;
        }

        $family = '';

        //Felder für Familenstand überprüfen und HTML zusammenbauen
        if (!empty($result['family'])) {
            $family .= $perRowStart.$perLeftCellStart.t('Familienstand').'<br>'.$cellEnd.$rightCellStart.$result['family'].'<br>'.$cellEnd.$rowEnd;
        }

        //HTML für persönliche Daten zusammenbauen
        $personalHTML = $tableStart;
        $personalHTML .= $userName;
        $personalHTML .= $birthday;
        $personalHTML .= $address;
        $personalHTML .= $email;
        $personalHTML .= $telephone;
        $personalHTML .= $nationality;
        $personalHTML .= $family;
        $personalHTML .= $tableEnd . '<br>';

    }

    //Persönliche Daten ins Array laden
    $content['cv']['personal_data'] = array(
        '#markup' => $personalHTML,
    );

    $practicalExpHTML = '';

    //Überprüfen, ob practical_experience in der Datenbank nicht leer ist
    if (!empty($result['practical_experience'])) {

        //String aus der DB in Array umwandeln
        $practicalExp = unserialize($result['practical_experience']);

        //Überprüfen, ob das Array Inhalt hat
        if (count($practicalExp) != 0) {

            //Überschrift definieren
            $practicalExpHTML .= '<div class="cv-headline">' . t('Berufserfahrungen') . '</div><br>';

            $practicalExpHTML .= $tableStart;

            //Array iterieren und HTML zusammenbauen
            foreach ($practicalExp as $value) {
                $practicalExpHTML .= $rowStart.$leftCellStart;

                if (!(empty($value['from_month_select']) && empty($value['from_year_select'])
                    && empty($value['to_month_select']) && empty($value['to_year_select']))) {

                    $practicalExpHTML .= $value['from_month_select'] . ' ';
                    $practicalExpHTML .= $value['from_year_select'] . ' - ';
                    $practicalExpHTML .= $value['to_month_select'] . ' ';
                    $practicalExpHTML .= $value['to_year_select'];
                }

                $practicalExpHTML .= '<br><br>'.$cellEnd.$rightCellStart;

                !empty($value['company_input']) ? $practicalExpHTML .= '<span class="weak-bold">' . t('Unternehmen: ') . '</span>' . $value['company_input'] . '<br>' : '';
                !empty($value['sector_input']) ? $practicalExpHTML .= '<span class="weak-bold">' . t('Branche: ') . '</span>' . $value['sector_input'] . '<br>' : '';
                !empty($value['employment_select']) ? $practicalExpHTML .= '<span class="weak-bold">' . t('Beschäftigungsart: ') . '</span>' . $value['employment_select'] . '<br>' : '';
                !empty($value['activity_input']) ? $practicalExpHTML .= '<span class="weak-bold">' . t('Tätigkeit: ') . '</span>' . $value['activity_input'] . '<br>' : '';
                !empty($value['work_description_input']) ? $practicalExpHTML .= '<span class="weak-bold">' . t('Beschreibung der Tätigkeit: ') . '</span>' . '<br>' . $value['work_description_input'] . '<br>' : '';

                $practicalExpHTML .= '<br>'.$cellEnd.$rowEnd;
            }

            $practicalExpHTML .= $tableEnd;
        }
    }

    //Praktische Erfahrungen ins Array laden
    $content['cv']['practical_experience_data'] = array(
        '#markup' => $practicalExpHTML,
    );

    $educationHTML = '';

    //Überprüfen, ob education in der Datenbank nicht leer ist
    if (!empty($result['education'])) {

        //String aus der DB in Array umwandeln
        $education = unserialize($result['education']);

        //Überprüfen, ob das Array Inhalt hat
        if (count($education) != 0) {
            //Überschrift definieren
            $educationHTML .= '<div class="cv-headline">' . t('Ausbildung') . '</div><br>';

            $educationHTML .= $tableStart;

            //Array iterieren und HTML zusammenbauen
            foreach ($education as $value) {
                $educationHTML .= $rowStart.$leftCellStart;

                if (!(empty($value['from_month_select']) && empty($value['from_year_select'])
                    && empty($value['to_month_select']) && empty($value['to_year_select']))) {

                    $educationHTML .= $value['from_month_select'] . ' ';
                    $educationHTML .= $value['from_year_select'] . ' - ';
                    $educationHTML .= $value['to_month_select'] . ' ';
                    $educationHTML .= $value['to_year_select'];
                }

                $educationHTML .= '<br><br>'.$cellEnd.$rightCellStart;

                !empty($value['training_center_input']) ? $educationHTML .= '<span class="weak-bold">' . t('Ausbildungsstätte: ') . '</span>' . $value['training_center_input'] . '<br>' : '';
                !empty($value['training_place_input']) ? $educationHTML .= '<span class="weak-bold">' . t('Ausbildungsort: ') . '</span>' . $value['training_place_input'] . '<br>' : '';
                !empty($value['education_art_select']) ? $educationHTML .= '<span class="weak-bold">' . t('Ausbildungsart: ') . '</span>' . $value['education_art_select'] . '<br>' : '';
                !empty($value['education_description_input']) ? $educationHTML .= '<span class="weak-bold">' . t('Zusätzliche Informationen zur Ausbildung: ') . '</span>' . '<br>' . $value['education_description_input'] . '<br>' : '';

                $educationHTML .= '<br>'.$cellEnd.$rowEnd;
            }

            $educationHTML .= $tableEnd;
        }
    }


    //Ausbildung ins Array laden
    $content['cv']['education_data'] = array(
        '#markup' => $educationHTML,
    );

    $languageSkillsHTML = '';

    //Überprüfen, ob language_skills in der Datenbank nicht leer ist
    if (!empty($result['language_skills'])) {

        //String aus der DB in Array umwandeln
        $languageSkills = unserialize($result['language_skills']);

        //Überprüfen, ob das Array Inhalt hat
        if (count($languageSkills) != 0) {
            //Überschrift definieren
            $languageSkillsHTML .= '<div class="cv-headline">' . t('Sprachkenntnisse') . '</div><br>';

            $languageSkillsHTML .= $tableStart;

            //Array iterieren und HTML zusammenbauen
            foreach ($languageSkills as $value) {
                $languageSkillsHTML .= $rowStart.$leftCellStart.'&nbsp;'.$cellEnd.$rightCellStart;

                !empty($value['language_input']) ? $languageSkillsHTML .= $value['language_input'] . ' ' : '';
                !empty($value['level_select']) ? $languageSkillsHTML .= '(' . $value['level_select'] . ')' : '';

                $languageSkillsHTML .= '<br>'.$cellEnd.$rowEnd;
            }

            $languageSkillsHTML .= $tableEnd . '<br>';
        }
    }

    //Sprachfähigkeiten ins Array laden
    $content['cv']['language_skills_data'] = array(
        '#markup' => $languageSkillsHTML,
    );

    $specialKnowledgeHTML = '';

    //Überprüfen, ob special_knowledge in der Datenbank nicht leer ist
    if (!empty($result['special_knowledge'])) {

        //String aus der DB in Array umwandeln
        $specialKnowledge = unserialize($result['special_knowledge']);

        //Überprüfen, ob das Array Inhalt hat
        if (count($specialKnowledge) != 0) {
            //Überschrift definieren
            $specialKnowledgeHTML .= '<div class="cv-headline">' . t('Weitere Fähigkeiten und Kenntnisse') . '</div><br>';

            $specialKnowledgeHTML .= $tableStart;

            //Array iterieren und HTML zusammenbauen
            foreach ($specialKnowledge as $value) {

                $specialKnowledgeHTML .= $rowStart.$leftCellStart.'&nbsp;'.$cellEnd.$rightCellStart;

                $specialKnowledgeHTML .= $value;

                $specialKnowledgeHTML .= '<br>'.$cellEnd.$rowEnd;
            }

            $specialKnowledgeHTML .= $tableEnd;
        }
    }

    //Weitere Fähigkeiten ins Array laden
    $content['cv']['special_knowledge_data'] = array(
        '#markup' => $specialKnowledgeHTML,
    );

    //Ende vom cv_container
    $content['cv']['cv_container_end'] = array(
        '#markup' => '</div>',
    );


    //Wenn der Benutzer seine eigene Seite aufruft oder ein Administrator dies tut, dann Buttons zum Editeren und Lebenslauf exportieren anzeigen
    if ($user_id == $user->uid || user_has_role(3, user_load($user->uid))) {
        $content['edit_button'] = array(
            '#markup' => '<br><a class="btn float-btn" href="' . base_path() . 'user/' . $user_id . '/usercv/edit">' . t('Editieren') . '</a>',
        );

        $content['print_pdf'] = array(
            '#markup' => '<a class="btn float-btn" target="_blank" href="' . base_path() . 'user/' . $user_id . '/usercv/pdf">' . t('PDF exportieren') . '</a><div class="clear-float"></div>',
        );
    }

    //Array zurückgeben, sodass dieses von Drupal ausgewertet und angezeigt werden kann
    return $content;
}

//Dompdf wird verwendet, um aus dem HTML Code vom Lebenslauf ein PDF zu erzeugen
use Dompdf\Dompdf;
use Dompdf\Options;

function print_usercv($user_id) {

    //Überprüfen, ob der Benutzer existiert
    $userLoad = user_load($user_id);
    if (!$userLoad) {
        drupal_not_found();
        drupal_exit();
    }

    global $user;

    //Überprüfen, ob der Benutzer selber seine Seite aufruft oder ein Administrator dieses tut. Andere Benutzer sollen das PDF nicht exportieren dürfen
    if ($user_id != $user->uid && !user_has_role(3, user_load($user->uid))) {
        drupal_access_denied();
        drupal_exit();
    }

    //Autoloader von dompdf laden
    require_once 'sites/all/libraries/dompdf/autoload.inc.php';

    //Lebenslauf anhand der ID laden
    $userCV = display_usercv($user_id);

    //Stylesheet laden, in HTML einbinden und das userCV Array rendern
    $stylesheet = file_get_contents(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css');
    $HTML = '<html>' .
        '<head>' .
        '<style>' . $stylesheet . '</style>' .
        '</head>' .
        '<body>' .
        render($userCV['cv']).
        '</body>' .
        '</html>';


    //dompdf instanziieren
    $dompdf = new Dompdf();

    //Optionen definieren und dompdf übergeben
    $options = new Options();
    $options->setIsRemoteEnabled(true);
    $dompdf->setOptions($options);

    //HTML in dompdf laden
    $dompdf->loadHtml($HTML);

    //HTML als PDF rendern
    $dompdf->render();

    //Ergebnis an den Browser senden. Bei Attachment = 0 wird das PDF im Browser angezeigt, falls möglich
    $dompdf->stream('Lebenslauf.pdf', array('compress' => 1, 'Attachment' => 0));

    //Funktion aufrufen, damit es nicht zur Warnung Header already sent kommt
    drupal_exit();

}
