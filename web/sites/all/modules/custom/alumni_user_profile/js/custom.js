/**
 * Created by Corin on 01.09.2017.
 */


jQuery(document).ready(function($) {

    $('.tabcontent:first').show();


    $('.tablinks').click(function () {
        $('.tabcontent').hide();

        $('.tablinks').removeClass('active');
        $(this).addClass('active');
        var target = '#' + $(this).attr('target');

        $(target).show();
    });

    $('.info-box-button').click(function () {
        $('.info-box').toggle('slow');
    });

});
/*function openCity(evt, cityName) {
    // Declare all variables
    var i, tabcontent, tablinks;

    // Get all elements with class="tabcontent" and hide them
    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }

    // Get all elements with class="tablinks" and remove the class "active"
    tablinks = document.getElementsByClassName("tablinks");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(" active", "");
    }

    // Show the current tab, and add an "active" class to the button that opened the tab
    document.getElementById(cityName).style.display = "block";
    evt.currentTarget.className += " active";
}*/