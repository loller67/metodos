import argparse
import time
import pandas as pd
import numpy as np
import csv
from notebooks import metnum
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.metrics import accuracy_score, recall_score, cohen_kappa_score, f1_score
import os

if not os.path.exists('../data/tests'):
    os.makedirs('../data/tests')


def run_knn(parameters, X_train, y_train, X_test, y_test,beta_used=None):
    knn = metnum.KNNClassifier(parameters.knn)

    knn.fit(X_train, y_train)
    y_predict = knn.predict(X_test)

    acc = accuracy_score(y_test, y_predict)
    print("Accuracy: {}".format(acc))
    
    if not parameters.allK:
        v = []
        ys = y_test.reset_index(drop=True)
        for x in range(0, X_test.shape[0]):
            v.append((ys[x], y_predict[x]))

        file = open(parameters.output, "w")
        for i, j in v:
            file.write("{},{}".format(int(i), int(j)))
            file.write("\n")
        file.close()

    if parameters.allK:
        data = {}
        ks = range(parameters.allKStart, parameters.allKEnd, parameters.allKStep)

        for k in ks:
            y_predict = [int(value) for value in knn.predictWithK(k)]

            acc         = accuracy_score(y_test, y_predict)
            # recall      = recall_score  (y_test, y_predict, labels=range(10), average=None)
            kappa_cohen = cohen_kappa_score(y_test, y_predict)
            # f1          = f1_score(y_test, y_predict, labels=range(10), average=None)

            # el average=None es porque tenemos un problema multiclass, para saber mas leer:
            # https://scikit-learn.org/stable/modules/classes.html#module-sklearn.metrics

            ####################################################
            # CALCULAR LAS OTRAS METRICAS Y AGREGARLAS A DATA  #
            # no olvidar agregarla en el write del archivo     #
            ####################################################
            
            data[k] = {
                'k': k,
                'accuracy': acc,
                'kappa_cohen': kappa_cohen,
                #'metrica': valor,
            }
            # for i in range(10):
            #     data[k]['recall_{}'.format(i)]  = recall[i]
            #     data[k]['f1score_{}'.format(i)] = f1[i]


        file_name = "{}_method{}_train{}_test{}_beta{}".format(
            parameters.output,
            parameters.method,
            X_train.shape[0],
            X_test.shape[0],
            beta_used, # beta
        )

        with open(file_name, 'w') as f:
            metricas = ['k', 'accuracy', 'kappa_cohen']    ## <===== hay que agregarlo aca!
            # metricas = metricas + ['recall_{}' .format(i) for i in range(10)]
            # metricas = metricas + ['f1score_{}'.format(i) for i in range(10)]

            w = csv.DictWriter(f, metricas)
            w.writeheader()
            for key, row in data.items():
                w.writerow(row)


    

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--method', type=int)
    parser.add_argument('-i', '--inputset')
    parser.add_argument('-q', '--queryset')
    parser.add_argument('-o', '--output')

    parser.add_argument('-k',               '--knn',             default=24,    type=int)                 #k vecinos
    parser.add_argument('-allK',            '--allK',            default=False, type=bool)
    parser.add_argument('-allKStart',       '--allKStart',       default=1,     type=int)
    parser.add_argument('-allKEnd',         '--allKEnd',         default=2000,  type=int)
    parser.add_argument('-allKStep',        '--allKStep',        default=2,     type=int)
    parser.add_argument('-beta',            '--pca',             default=55,    type=int)                #beta para pca
    parser.add_argument('-betaStart',       '--pcaStart',        default=0,     type=int)
    parser.add_argument('-betaEnd',         '--pcaEnd',          default=0,     type=int)
    parser.add_argument('-betaStep',        '--pcaStep',         default=0,     type=int)
    parser.add_argument('-percentageTrain', '--percentageTrain', default=0,     type=int)
    parser.add_argument('-percentageTest',  '--percentageTest',  default=0,     type=int)


    parameters = parser.parse_args()



    # si tiene porcentaje de train y test, entonces del input me quedo solo
    # con esos porcentjes
    if parameters.percentageTest and parameters.percentageTrain:
        df_original = pd.read_csv(parameters.inputset)#, index_col=0)
        offset      = (df_original.shape[0]*parameters.percentageTrain)//100
        offset_test = (df_original.shape[0]*parameters.percentageTest)//100
        # primeros offset para entrenar
        df    = df_original[:offset]
        # ultimos offset_test para testear
        df_test     = df_original[df_original.shape[0]-offset_test:]
    else:
        df = pd.read_csv(parameters.inputset)#, index_col=0)
        df_test = pd.read_csv(parameters.queryset)#, index_col=0)


    print("Nombre del Dataset: " + str(parameters.inputset) )
    print("Nombre del Testset: " + str(parameters.queryset) )

    print("Cantidad de Documentos: {}".format(df.shape[0]))
    print("Cantidad de Documentos a categorizar: {}".format(df_test.shape[0]))

    y_train = df["label"]
    X_train = df.drop(columns=['label'])

    y_test = df_test["label"]
    X_test = df_test.drop(columns=['label'])

    if parameters.method == 0:    #kNN solo

        print("Metodo utilizado: kNN" + "\n" + "k utilizado: " + str(parameters.knn))

        run_knn(parameters, X_train, y_train, X_test, y_test)

    elif parameters.method == 1:    #kNN+PCA

        print("Metodo utilizado: PCA + kNN" + "\n" + "beta utilizado: " + str(parameters.pca) + "\n" "k utilizado: " + str(parameters.knn))


        if parameters.allK and parameters.pcaStart != 0 and parameters.pcaEnd != 0 and parameters.pcaStep != 0:
            clf_pca = metnum.PCA(parameters.pcaEnd)
            clf_pca.fit(X_train)
            for beta in range(parameters.pcaStart, parameters.pcaEnd, parameters.pcaStep):

                X_train_pca = pd.DataFrame(data=clf_pca.transformBeta(X_train, beta))
                X_test_pca = pd.DataFrame(data=clf_pca.transformBeta(X_test, beta))

                run_knn(parameters, X_train_pca, y_train, X_test_pca, y_test, beta_used=beta)
        else:
            clf_pca = metnum.PCA(parameters.pca)
            clf_pca.fit(X_train)
            X_train_pca = clf_pca.transform(X_train)
            X_test_pca = clf_pca.transform(X_test)

            run_knn(parameters, X_train_pca, y_train, X_test_pca, y_test)
    else:
        print("cosas")
