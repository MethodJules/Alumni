/**
 * Created by Corin on 08.02.2018.
 */

jQuery(document).ready(function($) {

    var userPrivacyFields = $.parseJSON(Drupal.settings.privacyProfile.fieldsSettings);
    var userPrivacyOtherSettings = $.parseJSON(Drupal.settings.privacyProfile.otherSettings);
    var userID = $.parseJSON(Drupal.settings.privacyProfile.userID);

    console.log(userPrivacyOtherSettings);
    console.log(userPrivacyFields);
    if (!$.isEmptyObject(userPrivacyFields)) {
        $.each(userPrivacyFields, function(key, data) {
            var fieldName = key.replace(/\_/g, '-');
            $('.field-name-' + fieldName).after(data['content']);

            $('#' + fieldName + ' .form-select').val(data['status']);
        });
    }

    if (!$.isEmptyObject(userPrivacyOtherSettings)) {
        if (userPrivacyOtherSettings['paste']['position'] === 'before') {
            var fieldName = userPrivacyOtherSettings['paste']['field'].replace(/\_/g, '-');
            $('.field-name-' + fieldName).before(userPrivacyOtherSettings['content'] + '<br><hr><br>');
        } else if (userPrivacyOtherSettings['paste']['position'] === 'after') {
            var fieldName = userPrivacyOtherSettings['paste']['field'].replace(/\_/g, '-');
            $('.field-name-' + fieldName).after(userPrivacyOtherSettings['content'] + '<br><hr><br>');
        }

        if (!$.isEmptyObject(userPrivacyOtherSettings['privacy_config'])) {
            $.each(userPrivacyOtherSettings['privacy_config'], function (key, data) {
                var fieldName = key.replace(/\_/g, '-');
                var fieldChecked = true;

                if (data === '0') {
                    fieldChecked = false;
                }
                $('#' + fieldName + ' .form-checkbox').attr('checked', fieldChecked);
            });
        }

    }

    $(document).delegate('#user-profile-form', 'submit', function (e) {
        //e.preventDefault();

        var confArr = {};
        confArr['privacy_fields'] = {};
        confArr['privacy_others'] = {};

        $('.privacy-field-container').each(function (index) {
            var id = $(this).attr('id').replace(/\-/g, '_');

            confArr['privacy_fields'][id] = $(this).find('.form-select').val();
        });

        $('.privacy-other-container').each(function (index) {
            var id = $(this).attr('id').replace(/\-/g, '_');
            confArr['privacy_others'][id] = $(this).find('.form-checkbox').is(':checked');
        });

        $.ajax({
            type: 'POST',
            url: Drupal.settings.basePath + 'privacyprofile/config/save',
            dataType: 'json',
            data: {config: JSON.stringify(confArr), userid: userID},
            success:function(data) {
                //location.reload();
            },
        });

    });
});
