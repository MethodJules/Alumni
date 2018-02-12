/**
 * Created by Corin on 07.02.2018.
 */


jQuery(document).ready(function($) {

    $(document).delegate('.block-block_suggested_users ul.pager li a', 'click', function (e) {
        e.preventDefault();

        var page = $(this).attr('href').replace(/.*?page\=/, '');
        var thisURL = Drupal.settings.basePath + 'dashboard/get/suggested-users?page=' + page;

        $.ajax({
            type: 'GET',
            url: thisURL,
            success: function(data) {
                $('.block-block_suggested_users .portlet-content').html(data);
            },
        });

    });

});