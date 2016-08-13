function addArticle(dict_name, text) {
    var article = document.querySelector('#tpl-article').cloneNode(true);
    var container = document.querySelector('#container');
    article.innerHTML = article.innerHTML.replace(/{{index}}/g, container.childElementCount);
    article.innerHTML = article.innerHTML.replace(/{{name}}/g, dict_name);
    article.innerHTML = article.innerHTML.replace(/{{text}}/g, text);
    var fragment = document.importNode(article.content, true)
    container.appendChild(fragment);
}

function clearArticles() {
    var container = document.querySelector('#container');
    container.innerHTML = "";
}
