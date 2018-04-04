/**
 * Created by Corin on 16.02.2018.
 */

jQuery(document).ready(function($) {

    //Variablen definieren
    var openChatIDs = [];
    var openChatWindowIDs = [];
    var newMessagesPolled = [];

    var right = 230;
    var maxBoxPerPage = 0;
    var activeBoxID = 0;

    //Initial die Funktionen aufrufen
    calcMaxBoxPerPage();
    getFriendsList();
    getSettings();


    //Nach einer halben Sekunde soll der chatPoll ausgeführt werden
    setTimeout(function () {
        chatPoll();
    }, 500);

    //Die Funktion wird alle 3 Sekunden aufgerufen und holt neue Nachrichten und aktualisiert die Benutzer, die online sind
    function initChatPoll() {
        setTimeout(function () {
            chatPoll();
            getFriendsList()
        }, 3000);
    }

    //Funktion lädt die Nachrichten von einem Chat
    function getChatHistory(sender) {
        var thisURL = Drupal.settings.basePath + 'friends-chat/get-chat-history';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {sender: sender},
            success: function (data) {
                //Status überprüfen
                if (data.status === 'success') {

                    //Chatbox holen und Inhalt leeren
                    var chatbox = $('.friends-chatbox[boxid=' + data.sender + ']');
                    chatbox.find('.chatbox-content').html('');

                    //Alle Nachrichten iterieren und der Chatbox hinzufügen
                    for (var i = 0; i < data.messages.length; i++) {

                        var m = data.messages;
                        var date = getFormattedDate(m[i].created_timestamp*1000);

                        chatbox.find('.chatbox-content').append('<div class="' + m[i].type + '" timestamp="' + m[i].created_timestamp + '">' + m[i].message + '<span class="time">' + date + '</span></div>');
                        chatbox.find('.chatbox-content').scrollTop(chatbox.find('.chatbox-content')[0].scrollHeight);

                        //Überprüfen, ob die Nachricht bereits einmal abgeholt wurde. Falls dies nicht der Fall ist, dann soll die Chatbox blau hinterlegt werden und die Nachricht wird
                        //dem Array newMessagesPolled hinzugefügt
                        if (checkMessagePolled(data.sender, m[i].created_timestamp, newMessagesPolled) === -1 && m[i].read_timestamp === null && m[i].type === 'message-friend') {
                            chatbox.find('.chatbox-head').addClass('blink');
                            newMessagesPolled.push({sender: data.sender, timestamp: m[i].created_timestamp});
                        }
                    }
                }
            }
        });
    }

    //Funktion wird zyklisch aufgerufen und überprüft, ob es neue Nachrichten gibt
    function chatPoll() {

        var thisURL = Drupal.settings.basePath + 'friends-chat/chat-poll';
        $.ajax({
            type: 'POST',
            url: thisURL,
            success: function (data) {

                //Status überprüfen
                if (data.status === 'new') {
                    //Alle neuen Nachrichten iterieren
                    $.each(data.messages, function (i, obj) {

                        //Überprüfen, ob die Nachricht bereits einmal abgeholt wurde. Bei bereits abgeholten Nachrichten sollen diese nicht noch einmal geladen werden
                        //Nachrichten können bereits abgeholt worden sein, aber sind noch nicht gelesen worden, somit werden diese Nachrichten bei jedem chatPoll mit zurückgegeben
                        if (checkMessagePolled(obj.sender, obj.created_timestamp, newMessagesPolled) === -1) {

                            //Nachricht dem Array hinzufügen und per updateMessagesPolled das Array in der Datenbank speichern
                            newMessagesPolled.push({sender: obj.sender, timestamp: obj.created_timestamp});
                            updateMessagesPolled();

                            //Überprüfen, ob das Chatfenster bereits offen ist
                            if (checkIdExists(obj.sender, openChatIDs) === -1) {

                                //Info zur Chatbox den Arrays hinzufügen
                                var user = {id: obj.sender, name: obj.name};
                                openChatIDs.unshift(user);
                                openChatWindowIDs.unshift(user);

                                //Chatbox hinzufügen, Nachrichtenverlauf laden und die Bereite der Boxen neu berechnen
                                $('#friends-chat').prepend(createChatbox(obj.sender, obj.name));

                                getChatHistory(obj.sender);

                                calcMaxBoxPerPage();

                            }

                            //Chatbox holen und neue Nachrichten der Chatbox hinzufügen
                            var chatbox = $('.friends-chatbox[boxid=' + obj.sender + ']');

                            var date = getFormattedDate(obj.created_timestamp*1000);

                            if (obj.sender !== activeBoxID) {
                                chatbox.find('.chatbox-head').addClass('blink');
                            }
                            chatbox.find('.chatbox-content').append('<div class="message-friend" timestamp="' + obj.created_timestamp + '">' + obj.message + '<span class="time">' + date + '</span></div>');
                            chatbox.find('.chatbox-content').scrollTop(chatbox.find('.chatbox-content')[0].scrollHeight);

                        }

                    });

                    //Funktion atkualisiert die offenen Chats in der Datenbank
                    updateOpenChats();


                }
            }
        });

        //Funktion aufrufen, damit alle 3 Sekunden Nachrichten abgeholt werden
        initChatPoll();
    }

    //Funktion gibt einen String im Format dd.mm.yyyy zurück
    function getFormattedDate(timestamp) {

        var time;
        if (timestamp === '') {
            time = new Date();
        } else {
            time = new Date(timestamp);
        }

        var day = time.getDate() < 10 ? '0' + time.getDate() : time.getDate();
        var month = (time.getMonth() + 1) < 10 ? '0' + (time.getMonth() + 1) : (time.getMonth() + 1);
        var year = time.getFullYear();
        var hours = time.getHours() < 10 ? '0' + time.getHours() : time.getHours();
        var minutes = time.getMinutes() < 10 ? '0' + time.getMinutes() : time.getMinutes();
        var date = day + '.' + month + '.' + year + ' ' + hours + ':' + minutes;

        return date;
    }

    //Funktion übergibt per AJAX das Array, das dann in der Datenbank geschrieben wird
    function updateMessagesPolled() {
        var thisURL = Drupal.settings.basePath + 'friends-chat/update-messages-polled';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {messagesPolled: JSON.stringify(newMessagesPolled)},
            success: function (data) {
            }
        });
    }

    //Funktion wird aufgerufen, wenn eine Nachricht gelesen wurde und dies wird in der Datenbank gespeichert
    function setReadTimestamp(sender) {
        var thisURL = Drupal.settings.basePath + 'friends-chat/set-read-timestamp';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {sender: sender},
            success: function (data) {
            }
        });
    }

    //Funktion lädt aus der Datenbank die geöffneten Chats und die bereits abgeholten Nachrichten
    function getSettings() {
        var thisURL = Drupal.settings.basePath + 'friends-chat/get-settings';

        $.ajax({
            type: 'GET',
            url: thisURL,
            success: function (data) {

                //Länge des Ergebnisses überprüfen
                if (data.length !== 0) {

                    //Überprüfen, ob das Array definiert ist und die beiden Arrays dann in die lokalen Variablen speichern
                    if (typeof data.openChatIDs !== 'undefined' && data.openChatIDs.length > 0) {
                        openChatIDs = $.parseJSON(data.openChatIDs);
                        openChatWindowIDs = $.parseJSON(data.openChatWindowIDs);

                        //Array iterieren und die geöffneten Chatboxen aus dem Array anzeigen lassen
                        $.each(openChatIDs, function (index, data) {
                            $('#friends-chat').append(createChatbox(data.id, data.name));

                            if (checkIdExists(data.id, openChatWindowIDs) === -1) {
                                $('.friends-chatbox[boxid=' + data.id + ']').find('.chatbox-main').hide();
                            }

                            //Verlauf holen
                            getChatHistory(data.id);

                            //Funktion aufrufen, die die Breite der Chatfenster berechnet und diese nebeneinander darstellt
                            adjustChatboxRight();
                        });
                    }

                    //Array in lokale Variable laden
                    if (typeof data.messagesPolled !== 'undefined' && data.messagesPolled.length > 0) {
                        newMessagesPolled = $.parseJSON(data.messagesPolled);
                    }

                }
            }
        });
    }

    //Funktion holt zyklisch die Informationen über die Benutzer, ob diese online oder offline sind
    function getFriendsList() {
        var thisURL = Drupal.settings.basePath + 'friends-chat/get-friends-list';

        $.ajax({
            type: 'GET',
            url: thisURL,
            success: function(data) {
                //HTML leeren
                $('.friends-list .list-content').html('');

                //Falls keine Freunde vorhanden sind, soll eine entsprechende Nachricht in der Freundesliste hinzugefügt werden
                if (data['status'] === 'error') {
                    $('.friends-list .list-content').append(data['message']);
                }

                //Andernfalls durch das Array iterieren
                if (data['status'] === 'success') {
                    var count = 0;

                    //Array iterieren
                    $.each(data['users'], function (key, val) {
                        var html =
                            '<div class="friend">' +
                            '<div class="status ' + val.status + '"></div>' +
                            '<div class="name"><a href="#" class="' + key + '">' + val.name + '</a></div>' +
                            '</div>';

                        //Freund der Freundeliste hinzufügen
                        $('.friends-list .list-content').append(html);

                        //Wenn ein Chatfenster zu dem Freund geöffnet ist, soll dort der Status online oder offline angezeigt und aktualisiert werden
                        if ($('.friends-chatbox[boxid=' + key + ']').length > 0) {
                            $('.friends-chatbox[boxid=' + key + ']').find('.status').removeClass('online');
                            $('.friends-chatbox[boxid=' + key + ']').find('.status').removeClass('offline');
                            $('.friends-chatbox[boxid=' + key + ']').find('.status').addClass(val.status);
                        }
                        count++;
                    });

                    //Anzahl der Freunde anzeigen
                    $('.friends-list .list-head div').html('Chat (' + count + ')')
                }

            },
        });
    }

    //Wenn in das Chatfenster geklickt wird, dann sollen nicht gelesene Nachrichten als gelesen gespeichert werden
    $(document).delegate('.chatbox-content, .chatbox-input', 'click', function (e) {

        //Chatbox holen und blink entfernen
        var chatbox = $(this).parents('.friends-chatbox');
        chatbox.find('.chatbox-head').removeClass('blink');

        //ID holen
        activeBoxID = chatbox.attr('boxid');

        //Nun gelesene Nachrichten aus dem Array entfernen und setReadTimestamp aufrufen
        clearMessagesPolled(activeBoxID);
        setReadTimestamp(activeBoxID);
    });

    //Wenn außerhalb des Chatfensters geklickt wird, dann soll die aktive Box auf 0 gesetzt werden
    $(document).delegate('body', 'click', function (e) {
        if ($(e.target).parents('.friends-chatbox[boxid=' + activeBoxID + ']').length === 0) {
            activeBoxID = 0;
        }

    });

    //Wenn auf einen Freund in der Freundesliste geklickt wird, soll ein Chatfenster aufgerufen werden
    $(document).delegate('.friends-list .friend', 'click', function (e) {
        e.preventDefault();

        //Informationen holen
        var boxID = $(this).find('a').attr('class');
        var userName = $(this).find('a').html();

        var user = {id: boxID, name: userName};

        //Wenn die ID in openChatIDs bereits existiert, dann gibt es bereits ein Chatfenster
        if (checkIdExists(boxID, openChatIDs) !== -1) {
            //Das Chatfenster soll als erstes Chatfenster angezeigt werden. Daher die ID erst einmal aus den Arrays entferne
            openChatIDs.splice(checkIdExists(boxID, openChatIDs), 1);
            openChatWindowIDs.splice(checkIdExists(boxID, openChatWindowIDs), 1);

            //ID am Anfang der Arrays einfügen
            openChatIDs.unshift(user);
            openChatWindowIDs.unshift(user);

            //Chatfenster holen
            var element = $('.friends-chatbox[boxid=' + boxID + ']');

            //Chatfenster entfernen
            $('.friends-chatbox[boxid=' + boxID + ']').remove();

            //Chatfenster am Anfang einfügen und den Mainbereich mit den Nachrichten anzeigen
            $('#friends-chat').prepend(element);
            $('.friends-chatbox[boxid=' + boxID + ']').find('.chatbox-main').show();

        }

        //Wenn die ID in beiden Arrays nicht existiert, ein neues Chatfenster erstellen, am Anfang einfügen und die ID den Arrays hinzufügen
        if (checkIdExists(boxID, openChatIDs) === -1 && checkIdExists(boxID, openChatWindowIDs) === -1) {

            $('#friends-chat').prepend(createChatbox(boxID, userName));

            openChatIDs.unshift(user);
            openChatWindowIDs.unshift(user);

        }

        //Chatverlauf laden
        getChatHistory(boxID);

        //Funktion aufrufen, die die Breite der Chatfenster berechnet und diese nebeneinander darstellt
        adjustChatboxRight();

        //Fokus auf Eingabefeld
        $('.friends-chatbox[boxid=' + boxID + '] .chatbox-input .editor').focus();

        //Geöffnete Chats in der Datenbank aktualisieren
        updateOpenChats();

    });

    //ID und Liste wird übergeben und Funktion überprüft, ob die ID in der Liste vorhanden ist. Gibt entweder die Position der ID
    //in der Liste aus oder -1 wenn nicht vorhanden
    function checkIdExists(id, list) {
        var i;
        for (i = 0; i < list.length; i++) {
            if (list[i].id === id) {
                return i;
            }
        }

        return -1;
    }

    //Funktion überprüft, ob sender und der dazu passende timestamp in der Liste vorhanden sind und gibt Position in der Liste aus oder -1
    function checkMessagePolled(sender, timestamp, list) {
        var i;
        for (i = 0; i < list.length; i++) {
            if (list[i].sender === sender && list[i].timestamp === timestamp) {
                return i;
            }
        }

        return -1;
    }

    //Entfernt die ID aus der Liste newMessagesPolled
    function clearMessagesPolled(boxid) {

        newMessagesPolled = $.grep(newMessagesPolled, function (el, i) {
            if (el.sender === boxid) {
                return false;
            }

            return true;
        });

        //Datenbank aktualisieren
        updateMessagesPolled();
    }

    //Funktion erzeugt HTML-Gerüst für eine neue Chatbox
    function createChatbox(boxID, userName) {
        var boxHTML =
            '<div class="friends-chatbox" boxID="' + boxID + '">' +
            '<div class="panel">' +
            '<div class="chatbox-head"><div>' + userName + '</div></div>' +
            '<div class="chatbox-main">' +
            '<div class="chatbox-title">' +
            '<div class="status"></div>' +
            '<span class="close">x</span>' +
            userName +
            '</div>' +
            '<div class="chatbox-content">' +

            '</div>' +
            '<div class="chatbox-input">' +
            '<div class="editor" contenteditable="true"></div> ' +
            '</div>' +
            '</div>' +
            '</div>' +
            '</div>';

        return boxHTML;
    }

    //Funktion übergibt per AJAX das Array, das dann in die Datenbank geschrieben wird
    function updateOpenChats() {

        var thisURL = Drupal.settings.basePath + 'friends-chat/update-open-chats';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {openChatIDs: JSON.stringify(openChatIDs), openChatWindowIDs: JSON.stringify(openChatWindowIDs)},
            success: function (data) {
            }
        });
    }

    //Funktion berechnet die Breite der Chatfenster und positoniert diese nebeneinander. Wenn die maximale Breite des Fensters überschritten wird, werden die restlichen
    //Fenster ausgeblendet.
    function adjustChatboxRight () {
        right = 230;
        $('.friends-chatbox').each(function (index) {
            $(this).css('right', right);
            right += 250;

            if (index >= maxBoxPerPage) {
                $(this).hide();
            } else {
                $(this).show();
            }
        });
    }

    //Bestimmt die aktuelle Breite des Browserfensters und ruft adjustChatboxRight auf
    function calcMaxBoxPerPage () {
        maxBoxPerPage = Math.floor($(window).width() / 220) - 1;
        adjustChatboxRight();
    }

    //Funktioniert wird ausgelöst, wenn eine Taste gedrückt wird
    $(document).delegate('.chatbox-input .editor', 'keydown', function (e) {

        //Nur weiter gehen, wenn die Enter Taste ohne die Shift Taste gedrückt wird
        if (e.keyCode === 13 && event.shiftKey === false) {
            var message = $(this).html();
            //Leerzeilen vor und nach der Nachricht löschen, quasi ein trim
            message = message.replace(/^(<br\s*\/?>)*|(<br\s*\/?>)*$/ig, '');

            //Länge überprüfen
            if (message.length > 0) {

                //Request zusammenbauen
                var request = {
                    'receiver': $(this).parents('.friends-chatbox').attr('boxid'),
                    'message': message
                };

                //Nachricht an den Server senden und speichern
                var thisURL = Drupal.settings.basePath + 'friends-chat/save-message';
                $.ajax({
                    type: 'POST',
                    url: thisURL,
                    data: {content: JSON.stringify(request)},
                    success: function (data) {

                    }
                });

                //Datum holen und Nachricht der Chatbox hinzufügen
                var date = getFormattedDate('');

                var chatboxContent = $(this).parents('.chatbox-main').find('.chatbox-content');
                chatboxContent.append('<div class="message-me">' + message + '<span class="time">' + date + '</span></div>');
                chatboxContent.scrollTop(chatboxContent[0].scrollHeight);

            }

            //Eingabefeld leeren
            $(this).html('');
            return false;

        }
    });

    //Funktion schließt eine Chatbox
    $(document).delegate('.chatbox-main .chatbox-title .close', 'click', function (e) {
        e.stopPropagation();

        //Chatbox holen und aus den Arrays entfernen
        var box = $(this).parents('.friends-chatbox');
        openChatIDs.splice(checkIdExists(box.attr('boxid'), openChatIDs), 1);
        openChatWindowIDs.splice(checkIdExists(box.attr('boxid'), openChatWindowIDs), 1);

        //Eigentliche Chatbox entfernen, Breite und Anordnung der Boxen neu berechnen und die geöffneten Chats in der Datenbank aktualisieren
        box.remove();
        adjustChatboxRight();
        updateOpenChats();
    });

    //Funktion öffnet oder schließt die Freundesliste bei Klick
    $(document).delegate('.friends-list .list-head, .friends-list .list-title', 'click', function (e) {
        $(this).parents('.friends-list').find('.list-main').toggle();
    });

    //Funktion minimiert oder maximiert eine Chatbox
    $(document).delegate('.friends-chatbox .chatbox-head, .friends-chatbox .chatbox-title', 'click', function (e) {

        //Chatbox holen
        var box = $(this).parents('.friends-chatbox');

        //Weitere Informationen holen
        var boxID = box.attr('boxid');
        var userName = $('.friend a[class=' + boxID + ']').html();
        var user = {id: boxID, name: userName};

        //Chatbox maximieren oder minimieren
        if (box.find('.chatbox-main').is(':hidden')) {
            openChatWindowIDs.push(user);
            box.find('.chatbox-main').show();
        } else {
            openChatWindowIDs.splice(checkIdExists(box.attr('boxid'), openChatWindowIDs), 1);
            box.find('.chatbox-main').hide();
        }

        updateOpenChats();

    });

    //Beim Vergrößern des Fensters sollen die geöffneten Chats überprüft werden, ob die Breite der Chatboxen nicht die Breite des Browserfensters sprengen würde
    $(window).resize(function() {
        calcMaxBoxPerPage();
    });
});
