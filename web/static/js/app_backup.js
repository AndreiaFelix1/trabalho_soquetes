// ============================================================
// RESOURCE SERVER - DASHBOARD
// ============================================================


document.addEventListener("DOMContentLoaded", function () {

    createActivityChart();

    createOperationsChart();

});


// ============================================================
// ACTIVITY CHART
// ============================================================

function createActivityChart() {

    const canvas = document.getElementById("activityChart");

    if (!canvas) {
        return;
    }

    new Chart(canvas, {

        type: "line",

        data: {

            labels: [
                "00h",
                "02h",
                "04h",
                "06h",
                "08h",
                "10h",
                "12h",
                "14h",
                "16h",
                "18h"
            ],

            datasets: [

                {
                    label: "Comandos recebidos",

                    data: [
                        2,
                        4,
                        3,
                        5,
                        8,
                        6,
                        10,
                        7,
                        12,
                        9
                    ],

                    borderWidth: 2,

                    tension: 0.4,

                    fill: false
                },

                {
                    label: "Comandos bem-sucedidos",

                    data: [
                        2,
                        3,
                        3,
                        4,
                        7,
                        6,
                        9,
                        7,
                        11,
                        9
                    ],

                    borderWidth: 2,

                    tension: 0.4,

                    fill: false
                }

            ]

        },

        options: {

            responsive: true,

            maintainAspectRatio: false,

            interaction: {
                intersect: false,
                mode: "index"
            },

            plugins: {

                legend: {
                    display: true,

                    position: "bottom",

                    labels: {
                        boxWidth: 8,
                        font: {
                            size: 9
                        }
                    }
                }

            },

            scales: {

                x: {
                    grid: {
                        display: false
                    },

                    ticks: {
                        font: {
                            size: 8
                        }
                    }
                },

                y: {

                    beginAtZero: true,

                    ticks: {
                        precision: 0,

                        font: {
                            size: 8
                        }
                    },

                    grid: {
                        color: "#eef1f5"
                    }

                }

            }

        }

    });

}


// ============================================================
// OPERATIONS CHART
// ============================================================

function createOperationsChart() {

    const canvas = document.getElementById("operationsChart");

    if (!canvas) {
        return;
    }

    new Chart(canvas, {

        type: "doughnut",

        data: {

            labels: [
                "GET",
                "SET",
                "CREATE",
                "RESERVE",
                "OUTROS"
            ],

            datasets: [

                {

                    data: [
                        32,
                        24,
                        18,
                        14,
                        12
                    ],

                    borderWidth: 0

                }

            ]

        },

        options: {

            responsive: true,

            maintainAspectRatio: false,

            cutout: "68%",

            plugins: {

                legend: {
                    display: false
                }

            }

        }

    });

}
