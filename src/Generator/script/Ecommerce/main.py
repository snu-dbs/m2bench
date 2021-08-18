import sys
import customer_and_person
import graph_model
import relational_model
import document_model


"""  E-commerce Data Generator  """

def main(argv):
    tpc_ds_dirpath = argv[0]
    unibench_dirpath = argv[1]

    customer_and_person.generator(tpc_ds_dirpath)

    graph_model.edge_generator(unibench_dirpath)

    relational_model.product_generator(unibench_dirpath)
    relational_model.brand_generator(unibench_dirpath)

    document_model.order_generator(unibench_dirpath)
    document_model.review_generator()


if __name__ == '__main__':
    main(sys.argv[1:])