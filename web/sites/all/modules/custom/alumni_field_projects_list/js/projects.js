/**
 * Created by Corin on 31.01.2018.
 */

jQuery(document).ready(function($) {

    //Zeigt nach Klick die Autovervollständigungsliste an
    $(document).delegate('.change-to-autocomplete-list', 'click', function (e) {

        e.preventDefault();
        var parent = $(this).parent().parent();

        //Normale Auswahl ausblenden Autovervollständigungsliste einblenden
        parent.find('.normal-select-list').hide();
        parent.find('.autocomplete-list').show();

        //Dem hidden input Feld active-list die aktive Auswahlmöglichkeit zuweisen. Feld wird beim Speichern des Formulars ausgewertet
        parent.find('.active-list input').val('autocomplete-list');
    });

    //Zeigt nach Klick die normale Auswahlliste an
    $(document).delegate('.change-to-normal-select-list', 'click', function (e) {

        e.preventDefault();
        var parent = $(this).parent().parent();

        parent.find('.normal-select-list').show();
        parent.find('.autocomplete-list').hide();

        parent.find('.active-list input').val('normal-select-list');
    });
});