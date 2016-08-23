function addArticle(dict_name, text) {
    var article = document.querySelector('#tpl-article').cloneNode(true);
    var container = document.querySelector('#container');
    var index = container.childElementCount;
    article.innerHTML = article.innerHTML.replace(/{{index}}/g, index);
    article.innerHTML = article.innerHTML.replace(/{{name}}/g, dict_name);
    article.innerHTML = article.innerHTML.replace(/{{text}}/g, text);
    var fragment = document.importNode(article.content, true)
    container.appendChild(fragment);
    if (index == 0) $('#article0').collapse('show');
    var a = $('<a></a>').text(dict_name)
        .attr({
            href: '#article_heading' + index,
        })
        .click(function() {
            var article = $(this).attr('href').replace('_heading', '');
            $(article).collapse('show');
        });
    var li = $('<li></li>').append(a);
    $('.sidebar-nav').append(li);
}

function clearArticles() {
    var container = document.querySelector('#container');
    container.innerHTML = "";
    $('.sidebar-nav li:gt(0)').remove();
}


$(".sidebar-toggle").click(function(e) {
    e.preventDefault();
    if ($('span', this).hasClass("glyphicon-remove")) {
        $('.sidebar-toggle span').addClass('animated fadeOut');
        $('.sidebar-toggle span').removeClass('glyphicon-remove animated fadeOut');
        $('.sidebar-toggle span').addClass('glyphicon-menu-right animated fadeIn');
    } else {
        $('.sidebar-toggle span').addClass('animated fadeOut');
        $('.sidebar-toggle span').removeClass('glyphicon-menu-right animated fadeOut');
        $('.sidebar-toggle span').addClass('glyphicon-remove animated fadeIn');
    }
    $("#wrapper").toggleClass("toggled");
});
