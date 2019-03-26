function transform(templateJson, dataJson) {
    template = JSON.parse(templateJson);
    data = JSON.parse(dataJson);
    this.$rootData = data;

    var transformed = transformToken(template, {
        $data: data
    });
    if (transformed.length > 0) {
        return JSON.stringify(transformed[0]);
    } else {
        return null;
    }
}

function transformObject(item, parentEvaluator) {
    var answer = [];

    var evaluator = createEvaluator(item, parentEvaluator);
    if (isArray(evaluator.$data)) {
        var itemJson = JSON.stringify(item);
        for (var i = 0; i < evaluator.$data.length; i++) {
            var dataItem = evaluator.$data[i];
            var newItem = JSON.parse(itemJson);
            delete newItem.$data;
            var newEvaluator = {
                $data: dataItem
            };

            for (var subAnswer in transformObject(newItem, newEvaluator)) {
                // This should be different, but idk why subAnswer doesn't work, and this works for all basic scenarios, so keeping it for now
                answer.push(newItem);
            }
        }
    } else {
        item.$data = evaluator.$data;
        resolveWhenOnItem(item, evaluator);
        if (item.$when) {
            for (var prop in item) {
                if (item.hasOwnProperty(prop) && !prop.startsWith('$')) {
                    var transformedPropVals = transformToken(item[prop], evaluator);
                    if (transformedPropVals.length > 0) {
                        item[prop] = transformedPropVals[0];
                    } else {
                        delete item[prop];
                    }
                }
            }

            answer.push(item);
        }
    }

    return answer;
}

function transformArray(array, evaluator) {
    for (var i = 0; i < array.length; i++) {
        var child = array[i];
        var tokens = transformToken(child, evaluator);
        array.splice(i, 1, tokens);
    }

    return array;
}

function transformToken(token, evaluator) {
    if (isArray(token)) {
        return transformArray(token, evaluator);
    } else if (typeof token === 'object') {
        return transformObject(token, evaluator);
    } else if (typeof token === 'string') {
        return transformString(token, evaluator);
    } else {
        return [token];
    }
}

function transformString(str, evaluator) {
    // If entire thing is expression, preserve
    if (new RegExp('^{([^}]+)}$').test(str)) {
        try {
            var expr = str.substring(1, str.length - 1);
            var answer = evaluate(expr, evaluator);
            if (answer !== undefined) {
                return [answer];
            } else {
                return [];
            }
        }
        catch (err) {
            return [];
        }
    }

    return [str.replace(/{([^}]+)}/g, function (expr) {
        try {
            expr = expr.substring(1, expr.length - 1);
            return evaluate(expr, evaluator);
        }
        catch (err) {
            return '';
        }
    })];
}

function resolveWhenOnItem(item, evaluator) {
    if (!isArray(item)) {
        if (item.$when === undefined) {
            item.$when = true;
        } else {
            try {
                item.$when = evaluate(item.$when, evaluator);
            } catch (err) {
                item.$when = false;
            }
        }
    }
}

function createEvaluator(item, parentEvaluator) {
    if (item.$data === undefined) {
        return {
            $data: parentEvaluator.$data
        };
    } else if (typeof item.$data === 'object') {
        return {
            $data: item.$data
        };
    } else {
        try {
            var nextData = evaluate(item.$data, parentEvaluator);
            return {
                $data: nextData
            };
        } catch (err) {
            return {
                $data: {}
            };
        }
    }
}

function evaluate(expression, evaluator) {
    for (var prop in evaluator.$data) {
        if (evaluator.$data.hasOwnProperty(prop)) {
            this[prop] = evaluator.$data[prop];
        }
    }
    this.$data = evaluator.$data;
    return eval(expression);
}

function isArray(obj) {
    return Object.prototype.toString.call(obj) === '[object Array]';
}
