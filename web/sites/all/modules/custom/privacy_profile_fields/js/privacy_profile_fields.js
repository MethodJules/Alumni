/**
 * Created by Corin on 08.02.2018.
 */

jQuery(document).ready(function($) {

    //Variablen laden, die in der PHP Datei definiert wurden
    var userPrivacyFields = $.parseJSON(Drupal.settings.privacyProfile.fieldsSettings);
    var userPrivacyOtherSettings = $.parseJSON(Drupal.settings.privacyProfile.otherSettings);
    var userID = $.parseJSON(Drupal.settings.privacyProfile.userID);

    //JSON Array iterieren, wenn Inhalt vorhanden ist
    if (!$.isEmptyObject(userPrivacyFields)) {
        //Array iterieren und anschließend nach jedem Feld ein Selectfeld für die Datenschutzeinstellungen hinzufügen
        $.each(userPrivacyFields, function(key, data) {
            var fieldName = key.replace(/\_/g, '-');
            $('.field-name-' + fieldName).after(data['content']);

            //Gespeicherten Status beim Selectfeld setzen, ob Öffentlich, Freunde oder nur ich
            $('#' + fieldName + ' .form-select').val(data['status']);
        });
    }

    //JSON Array iterieren, wenn Inhalt vorhanden ist
    if (!$.isEmptyObject(userPrivacyOtherSettings)) {
        //Überprüfen, ob die weiteren Datenschutzeinstellungen vor oder hinter einem, in den Einstellungen im Admininterface definierten Feld
        //hinzugefügt werden sollen.
        if (userPrivacyOtherSettings['paste']['position'] === 'before') {
            var fieldName = userPrivacyOtherSettings['paste']['field'].replace(/\_/g, '-');
            $('.field-name-' + fieldName).before(userPrivacyOtherSettings['content'] + '<br><hr><br>');
        } else if (userPrivacyOtherSettings['paste']['position'] === 'after') {
            var fieldName = userPrivacyOtherSettings['paste']['field'].replace(/\_/g, '-');
            $('.field-name-' + fieldName).after(userPrivacyOtherSettings['content'] + '<br><hr><br>');
        }

        //JSON Array iterieren, wenn Inhalt vorhanden ist
        if (!$.isEmptyObject(userPrivacyOtherSettings['privacy_config'])) {
            $.each(userPrivacyOtherSettings['privacy_config'], function (key, data) {
                var fieldName = key.replace(/\_/g, '-');

                //Überprüfen, ob der Wert aus der DB null oder eins ist und dementsprechend die Variable setzen
                var fieldChecked = true;

                if (data === '0') {
                    fieldChecked = false;
                }

                //Checkbox setzen oder nicht, abhängig von der zuvor definierten Variable
                $('#' + fieldName + ' .form-checkbox').attr('checked', fieldChecked);
            });
        }

    }

    //Ausführen, wenn das Formular im Benutzerprofil gespeichert wird
    $(document).delegate('#user-profile-form', 'submit', function (e) {
        //e.preventDefault();

        var confArr = {};
        confArr['privacy_fields'] = {};
        confArr['privacy_others'] = {};

        //Alle Felder mit Datenschutzeinstellungen iterieren und die Werte der Select- und Checkboxfelder auslesen und in einem Array speichern
        $('.privacy-field-container').each(function (index) {
            var id = $(this).attr('id').replace(/\-/g, '_');

            confArr['privacy_fields'][id] = $(this).find('.form-select').val();
        });

        $('.privacy-other-container').each(function (index) {
            var id = $(this).attr('id').replace(/\-/g, '_');
            confArr['privacy_others'][id] = $(this).find('.form-checkbox').is(':checked');
        });

        //Ausgelesenes Array per AJAX Post an Link senden, wo die Daten dann in der DB gespeichert werden.
        $.ajax({
            type: 'POST',
            url: Drupal.settings.basePath + 'privacyprofile/config/save',
            dataType: 'json',
            data: {config: JSON.stringify(confArr), userid: userID},
            success:function(data) {
            },
        });

    });
});
