/**
 * Created by Corin on 31.01.2018.
 */

jQuery(document).ready(function($) {

    $(document).delegate('.change-to-autocomplete-list', 'click', function (e) {

        e.preventDefault();
        var parent = $(this).parent().parent();

        parent.find('.normal-select-list').hide();
        parent.find('.autocomplete-list').show();

        parent.find('.active-list input').val('autocomplete-list');
    });

    $(document).delegate('.change-to-normal-select-list', 'click', function (e) {

        e.preventDefault();
        var parent = $(this).parent().parent();

        parent.find('.normal-select-list').show();
        parent.find('.autocomplete-list').hide();

        parent.find('.active-list input').val('normal-select-list');
    });
});