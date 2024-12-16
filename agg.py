import json

def avg(sum: float | int, length: int):
    if not length:
        return 0.0
    return round(sum / length, 3)

with open("parsed.json", "r") as file:
    parsed = json.load(file)
    
agg = []

for test_group in parsed:
    conf = test_group["conf"]
    data = test_group["data"]
    
    preprocess = {}

    keys = data["keys"]
    values = data["values"]
    keys_to_agg = [
        "add_or_remove_time", 
        "f1", 
        "f2", 
        "f3", 
        "f4", 
        "f5", 
        "accuracy", 
        "test_time",
    ]
    for value in values:
        (
            test_case, add_or_remove_time, 
            key_set, non_key_set, 
            f1, f2, f3, f4, f5, 
            test_size, accuracy, test_time
        ) = value

        data_to_agg = {
            "add_or_remove_time": add_or_remove_time, 
            "f1": f1, 
            "f2": f2, 
            "f3": f3, 
            "f4": f4, 
            "f5": f5, 
            "accuracy": accuracy, 
            "test_time": test_time,
        }

        key = f"{test_case}___{json.dumps(key_set)}___{json.dumps(non_key_set)}"
        
        if key not in preprocess:
            preprocess[key] = {
                k: []
                for k 
                in keys_to_agg
            }

        for k, v in data_to_agg.items():
            preprocess[key][k].append(v)

    
    for key_str, grouped in preprocess.items():
        (test_case, key_set, non_key_set) = key_str.split("___")
        key_set = json.loads(key_set)
        non_key_set = json.loads(non_key_set)
        subagg = {}
        count = 0
        for k, v in grouped.items():
            # k is the name of attribute
            # v is array of attribute values
            len_v = len(v)

            max_v = max(v)
            min_v = min(v)
            sum_v = sum(v)
            avg_v = avg(sum_v, len_v)
            count = len_v
            
            subagg[k] = {
                "max": max_v,
                "min": min_v,
                "sum": sum_v,
                "avg": avg_v,
            }
            
        
        agg.append({
            "meta": {
                **conf, 
                "TestCase": test_case, 
                "KeySet": key_set, 
                "NonKeySet": non_key_set, 
                "Count": count,
            },
            "value": subagg,
        })
        
    print(f"Aggregated for test group: {list(conf.values())}")
    
with open("agg.json", "w") as file:
    json.dump(agg, file, indent=2)
