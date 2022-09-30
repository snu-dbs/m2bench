import os
import json
import csv
import random
from pathlib import Path


def order_generator(unibench_dirpath, outdir_path):
    order_dir_path = "Order/"
    outdir_path = outdir_path+"/"

    f = open(Path(outdir_path+"table/")/'Customer.csv','r')
    rdr = csv.reader(f, delimiter='|')
    cid=[]
    for line in rdr:
        cid.append(line[0])
    cid.pop(0)

    pid=[]
    json_file = unibench_dirpath + order_dir_path +"Order.json"
    with open(json_file) as pid_for_json:
        for line in pid_for_json:
            json_decoded=json.loads(line)
            pid.append(int(json_decoded['PersonId']))
    pid = list(set(pid))
    pid.sort()

    data=[]
    index=0
    with open(json_file) as json_file:
        for line in json_file:
            json_decoded=json.loads(line)
            json_decoded['order_id'] = json_decoded.pop('OrderId')

            person_id = int(json_decoded['PersonId'])
            index = pid.index(person_id)
            json_decoded['customer_id'] = cid[index]
            # json_decoded['customer_id'] = cid[index % 9948]
            # index=index+1

            json_decoded['order_date'] = json_decoded.pop('OrderDate')
            json_decoded['total_price'] = json_decoded.pop('TotalPrice')
            json_decoded['order_line'] = json_decoded.pop('Orderline')
            del json_decoded['PersonId']

            data.append(json_decoded)

    order_file= Path(outdir_path+"json/") / "order.json"
    with open(order_file, 'w') as file:
        for d in data:
            orderline = d['order_line']
            for o in orderline:
                o['product_id'] = o.pop('asin')
                o['title'] = o.pop('title')
                o['price'] = o.pop('price')
                o.pop('productId')
                o.pop('brand')

            json.dump(d, file)
            file.write("\n")


def review_generator(outdir_path):
    outdir_path = outdir_path+"/"
    outputfile = "oid_pid.csv"
    f = open(outputfile, 'w')
    csvwriter = csv.writer(f)

    data=[]
    order_file= Path(outdir_path+"json/") / "order.json"
    with open(order_file) as file:
        for line in file:
            data.append(json.loads(line))

    for eachjson in data:
        row = []
        oid = eachjson['order_id']
        row.append(oid)
        for p in eachjson['order_line']:
            row.append(p['product_id'])
        csvwriter.writerow(row)


    feedback1 = ["I like it. My friend bought it, too.", "The product arrived very fast and I'm very satified with the overall quality of it.", "Exceptional value for money. Comfortable. Easty to use."]
    feedback2 = ["The product isn't bad but... I don't think I will recommend others.", "I wish the design would come in various colors.", "Hmmm, I've used it less than a month and so far so good."]
    feedback3 = ["To be honest, it's a huge waste of money.", "Within a week it is broken... I want to get a refund!", "Rubbish.. I should have read other reviews first before buying this."]
    
    jsonfile = open(Path(outdir_path+"json/") / "review.json", 'w')
    with open("oid_pid.csv") as oid_pid:
        rdr = csv.reader(oid_pid)
        file_header = next(rdr, None)

        for each in rdr:
            oid = each[0]
            pid = each[1]
            num = random.randint(1,5)
            row = {"order_id": oid, "product_id": pid, "rating": num}
            row_dict = json.dumps(row)
            jsonfile.write(row_dict)
            jsonfile.write("\n")

            if len(each) > 5:
                oid = each[0]
                pid = each[5]
                num = random.randint(1,5)
                if(num>=4): eval = feedback1[random.randint(0,2)]
                elif (num>=2): eval = feedback2[random.randint(0,2)]
                elif (num<=1): eval = feedback3[random.randint(0,2)]
                row = {"order_id": oid, "product_id": pid, "rating": num, "feedback": eval}
                row_dict = json.dumps(row)
                jsonfile.write(row_dict)
                jsonfile.write("\n")

    os.remove("oid_pid.csv")
