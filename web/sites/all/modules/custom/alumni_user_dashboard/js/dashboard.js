jQuery(document).ready(function($) {

    var jobOffersPage = 1;
    var suggestedUsersPage = 1;
    var parent;

    //sortable für die Liste aufrufen. Beim Ziehen eines Listelements wird die Größe des Platzhalters genauso gesetzt,
    //wie die Größe des eigentlichen Listelements
    $('#dashboard-sortable').sortable({
        placeholder: "ui-state-highlight",
        start: function(e, ui){
            ui.placeholder.height(ui.item.height());
        }
    });

    $('#dashboard-sortable').disableSelection();

    //Sortierung wieder deaktivieren, damit diese über das Zahnradsymbol ein-/ausgeschaltet werden kann
    $('#dashboard-sortable').sortable('disable');

    //Variable definieren, ob die Konfiguration, sprich das Zahnradsymbol gerade aktiv ist
    var configEnabled = false;

    //Button Klick des Zahnradsymbols
    $(document).delegate('.edit-dashboard-config', 'click', function (e) {

        //Wenn configEnabled auf false steht, den Konfigurationsblock anzeigen, Sortierung aktivieren, etc.
        if (configEnabled === false) {
            $('.dashboard-config-block').css('display', 'inline-block');

            $('#dashboard-sortable').sortable('enable');
            $('#dashboard-sortable li').css('cursor', 'move');
            configEnabled = true;
        } else {
            //Andernfalls den Konfigurationsblock ausblenden, Sortierung deaktivieren, etc.
            $('.dashboard-config-block').css('display', 'none');

            $('#dashboard-sortable').sortable('disable');
            $('#dashboard-sortable li').css('cursor', 'auto');
            configEnabled = false;
        }

    });

    //Funktion zum Speichern der aktuellen Konfiguration der Widgets/Blöcke
    //Json Array zusammenbauen mit den benötigten Informationen und per AJAX Post abschicken
    $(document).delegate('.save-dashboard-config', 'click', function (e) {

        var confArr = {};
        e.preventDefault();

        $('.widget-item').each(function (index) {

           confArr[$(this).attr('id')] = {
               itemPosition: index,
               itemActive: $('#' + $(this).attr('check')).is(':checked')
           };

        });

        $.ajax({
            type: 'POST',
            url: 'user-dashboard/config/save',
            dataType: 'json',
            data: {content: JSON.stringify(confArr)},
            success:function(data) {
                location.reload();
            },
        });
    });

    $(document).delegate('.back-btn', 'click', function (e) {

        e.preventDefault();
        parent = $(this).parent();

        var destination = '';
        var currentWidgetPage = 1;

        if (parent.attr('class') === 'widget-job-offers') {
            destination = 'user-dashboard/get/job-offers';
            jobOffersPage--;
            currentWidgetPage = jobOffersPage;
        }

        if (parent.attr('class') === 'widget-suggested-users') {
            destination = 'user-dashboard/get/suggested-users';
            suggestedUsersPage--;
            currentWidgetPage = suggestedUsersPage;
        }

        $.ajax({
            type: 'POST',
            url: destination,
            dataType: 'json',
            data: {page: currentWidgetPage},
            success:function(data) {
                parent.html(data);
            },
        });

    });

    $(document).delegate('.next-btn', 'click', function (e) {

        e.preventDefault();
        parent = $(this).parent();

        var destination = '';
        var currentWidgetPage = 1;

        if (parent.attr('class') === 'widget-job-offers') {
            destination = 'user-dashboard/get/job-offers';
            jobOffersPage++;
            currentWidgetPage = jobOffersPage;
        }

        if (parent.attr('class') === 'widget-suggested-users') {
            destination = 'user-dashboard/get/suggested-users';
            suggestedUsersPage++;
            currentWidgetPage = suggestedUsersPage;
        }

        $.ajax({
            type: 'POST',
            url: destination,
            dataType: 'json',
            data: {page: currentWidgetPage},
            success:function(data) {
                parent.html(data);
            },
        });

    });




});