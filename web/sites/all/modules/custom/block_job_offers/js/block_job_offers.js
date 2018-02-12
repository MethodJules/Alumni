/**
 * Created by Corin on 07.02.2018.
 */


jQuery(document).ready(function($) {

    $(document).delegate('.block-block_job_offers ul.pager li a', 'click', function (e) {
        e.preventDefault();

        var page = $(this).attr('href').replace(/.*?page\=/, '');
        var thisURL = Drupal.settings.basePath + 'dashboard/get/job-offers?page=' + page;

        $.ajax({
            type: 'GET',
            url: thisURL,
            success: function(data) {
                $('.block-block_job_offers .portlet-content').html(data);
            },
        });

    });

});